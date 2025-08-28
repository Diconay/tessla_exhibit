#include "lvgl/src/lvgl.h"
#include "lvgl/src/drivers/display/fb/lv_linux_fbdev.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <poll.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <ctype.h>

#define DEFAULT_FBDEV   "/dev/fb0"
#define DEFAULT_SOCK    "/tmp/hmi.sock"

static lv_obj_t *g_led = NULL;

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

static void lv_linux_disp_init(void){
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();

    lv_linux_fbdev_set_file(disp, device);
}

static void set_led_state(int on) {
    if (on) lv_led_on(g_led);
    else lv_led_off(g_led);
}

static void lv_ui_init(void){
    g_led = lv_led_create(lv_screen_active());
    lv_obj_set_size(g_led, 40, 40);
    lv_obj_set_style_radius(g_led, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(g_led, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_bg_opa(g_led, LV_OPA_20, 0);
    lv_obj_align(g_led, LV_ALIGN_CENTER, 0, 0);
    set_led_state(0);
}

// ============ UDS =============
static int set_nonblock(int fd) {
    int fl = fcntl(fd, F_GETFL, 0);
    if(fl < 0) return -1;
    return fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

static int create_uds_server(const char *path) {
    unlink(path); // убрать старый файловый сокет, если остался

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0) { perror("socket"); return -1; }

    if(set_nonblock(fd) < 0) { perror("fcntl(O_NONBLOCK)"); close(fd); return -1; }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);

    if(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(fd); return -1;
    }

    chmod(path, 0666); // на время разработки разрешим всем

    if(listen(fd, 4) < 0) {
        perror("listen"); close(fd); unlink(path); return -1;
    }
    return fd;
}

// trim CR/LF справа
static void rstrip(char *s) {
    size_t n = strlen(s);
    while(n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

// Разбор строки: ... TAG=blink ... VAL=0|1 ...
// Допускаем "TAG=" или опечатку "TEG="; регистр имени тега игнорируем.
static void handle_line(const char *line) {
    const char *ptag = strstr(line, "TAG=");
    if(!ptag) ptag = strstr(line, "TEG="); // на всякий случай
    const char *pval = strstr(line, "VAL=");
    if(!pval) return;

    // Считать tag до первого пробела
    char tag[128] = {0};
    if(ptag) {
        ptag += 4;
        size_t i = 0;
        while(ptag[i] && !isspace((unsigned char)ptag[i]) && i < sizeof(tag)-1) {
            tag[i] = ptag[i];
            i++;
        }
        tag[i] = '\0';
    }

    int v = atoi(pval + 4);
    if(tag[0] && strcasecmp(tag, "blink") == 0) {
        set_led_state(v != 0);
    }
}

int main(void){
    lv_init();
    lv_linux_disp_init();
    lv_ui_init();
    
    // 2) UDS server
    const char *sock_path = getenv_default("HMI_SOCK", DEFAULT_SOCK);
    int srv_fd = create_uds_server(sock_path);

    int cli_fd = -1;
    char rxbuf[1024];
    size_t rxlen = 0;

    /* Бесконечный цикл отрисовки */
    while(1) {
        struct pollfd fds[2];
        nfds_t nfds = 0;

        if(srv_fd >= 0) { fds[nfds].fd = srv_fd; fds[nfds].events = POLLIN; nfds++; }
        if(cli_fd >= 0) { fds[nfds].fd = cli_fd; fds[nfds].events = POLLIN | POLLHUP | POLLERR; nfds++; }

        (void)poll(fds, nfds, 5); // короткий таймаут, чтобы крутить LVGL

        // Accept
        if(srv_fd >= 0 && nfds >= 1 && (fds[0].fd == srv_fd) && (fds[0].revents & POLLIN)) {
            int cfd = accept(srv_fd, NULL, NULL);
            if(cfd >= 0) {
                set_nonblock(cfd);
                if(cli_fd >= 0) close(cli_fd);
                cli_fd = cfd;
                rxlen = 0;
            }
        }

        // Read lines
        if(cli_fd >= 0) {
            int idx = (srv_fd >= 0) ? 1 : 0;
            if(nfds > idx && fds[idx].fd == cli_fd && (fds[idx].revents & (POLLIN | POLLHUP | POLLERR))) {
                char tmp[256];
                ssize_t n = read(cli_fd, tmp, sizeof(tmp));
                if(n > 0) {
                    for(ssize_t i = 0; i < n; ++i) {
                        if(rxlen < sizeof(rxbuf)-1) rxbuf[rxlen++] = tmp[i];
                        if(tmp[i] == '\n') {
                            rxbuf[rxlen] = '\0';
                            rstrip(rxbuf);
                            handle_line(rxbuf);
                            rxlen = 0;
                        }
                    }
                } else if(n == 0) {
                    close(cli_fd); cli_fd = -1;
                } else if(errno != EAGAIN && errno != EWOULDBLOCK) {
                    close(cli_fd); cli_fd = -1;
                }
            }
        }

        lv_timer_handler();
        usleep(5000);
    }

    // недостижимо
    if(cli_fd >= 0) close(cli_fd);
    if(srv_fd >= 0) { close(srv_fd); unlink(sock_path); }
    return 0;
}
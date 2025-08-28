#include "lvgl/src/lvgl.h"
#include "lvgl/src/drivers/display/fb/lv_linux_fbdev.h"
#include <pthread.h>
#include <unistd.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define DEFAULT_FBDEV   "/dev/fb0"

#define COLOR_GRAFIT1   0x494d4e
#define COLOR_GRAFIT2   0x3b3b48
#define COLOR_TEXT      0x8f8f8f

#define FPGA_DEVNODE "/dev/fpga_ctrl0"

struct fpga_reg_io { uint32_t offset; uint32_t value; };
#define FPGA_IOC_MAGIC   'f'
#define FPGA_IOC_READ32  _IOWR(FPGA_IOC_MAGIC, 0x00, struct fpga_reg_io)

static const uint32_t reg_offsets[] = {0x34, 0x38};
static const char *key_names[] = {
    "Start", "Mode", "Cancel", "Left", "Stop", "Menu", "Up", "Down",
    "Reset", "Data", "Enter", "Right"
};

static void lv_linux_disp_init(void){
    lv_display_t * disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, DEFAULT_FBDEV);
}

static void poll_keys(int fd, lv_obj_t *label)
{
    struct fpga_reg_io io;
    uint32_t regs[2] = {0};
    for (unsigned i = 0; i < 2; i++) {
        io.offset = reg_offsets[i];
        io.value = 0;
        if (ioctl(fd, FPGA_IOC_READ32, &io) == 0)
            regs[i] = io.value;
    }

    static uint8_t prev[12];
    char buf[128] = "";
    for (unsigned i = 0; i < 12; i++) {
        unsigned reg_idx = i / 8;
        unsigned shift = (i % 8) * 4;
        uint8_t cnt = (regs[reg_idx] >> shift) & 0xF;
        if (cnt != prev[i]) {
            if (cnt > prev[i]) {
                if (buf[0]) strcat(buf, " ");
                strcat(buf, key_names[i]);
            }
            prev[i] = cnt;
        }
    }
    if (buf[0])
        lv_label_set_text(label, buf);
}

static void * tick_thread(void * data)
{
    (void) data;
    while(1) {
        lv_tick_inc(1);
        usleep(1000);
    }
    return NULL;
}

int main(void) {
    lv_init();
    lv_linux_disp_init();

    pthread_t tick_th;
    pthread_create(&tick_th, NULL, tick_thread, NULL);

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(COLOR_GRAFIT1), LV_PART_MAIN);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Main_2");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    int fd = open(FPGA_DEVNODE, O_RDONLY);
    if (fd < 0) {
        lv_label_set_text(label, "fpga open error");
        return 1;
    }

    while (1)
    {
        lv_timer_handler();
        poll_keys(fd, label);
        usleep(5000);
    }
    close(fd);
}
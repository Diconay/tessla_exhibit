#include "lvgl/src/lvgl.h"
#include "lvgl/src/drivers/display/fb/lv_linux_fbdev.h"
#include <pthread.h>
#include <unistd.h>

#include "ui.h"
#include "keyboard.h"
#include <stdbool.h>

#define DEFAULT_FBDEV   "/dev/fb0"

static void lv_linux_disp_init(void) {
    lv_display_t * disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, DEFAULT_FBDEV);
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

    bool sim_running = false;
    int32_t gen_kw = 0;

int main(void) {
    lv_init();
    lv_linux_disp_init();

    pthread_t tick_th;
    pthread_create(&tick_th, NULL, tick_thread, NULL);

    struct ui ui;
    ui_init(&ui);

    struct keyboard kb;
    if (keyboard_init(&kb) != 0) {
        // TODO print consol fpga open error
        return 1;
    }
    lv_screen_load(ui.screen_mode);
    while (1)
    {
        lv_timer_handler();
        keyboard_poll(&kb);

        if (kb.state.start)
            sim_running = true;
        if (kb.state.stop) {
            sim_running = false;
            gen_kw = 0;
        }

        if (sim_running) {
            gen_kw += 5;
            if (gen_kw > 1000)
                gen_kw = 1000;
        }

        if (kb.state.mode) lv_screen_load(ui.screen_mode);
        if (kb.state.menu) lv_screen_load(ui.screen_menu);
        if (kb.state.data) lv_screen_load(ui.screen_data);
        
        usleep(5000);
    }
    keyboard_deinit(&kb);
    return 0;
}
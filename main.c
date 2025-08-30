#include "lvgl/src/lvgl.h"
#include "lvgl/src/drivers/display/fb/lv_linux_fbdev.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

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
    lv_value_precise_t grid_freq = 50.0f;


int main(void) {
    lv_init();
    lv_linux_disp_init();

    srand(time(NULL));

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
            
        }

        if (sim_running) {
            gen_kw += 1;
            if (gen_kw > 1000)
                gen_kw = 1000;
        } else if (gen_kw > 0) {
            gen_kw -= 0.1;
        } else {
            gen_kw = 0;
        }
        lv_value_precise_t delta = ((lv_value_precise_t)(rand() % 11) - 5.0f) / 1000.0f;
        grid_freq += delta;
        if (grid_freq > 50.05f)
            grid_freq = 50.05f;
        if (grid_freq < 49.95f)
            grid_freq = 49.95f;

        ui_update_power(gen_kw);
        ui_update_freq(grid_freq);
        

        if (kb.state.mode) lv_screen_load(ui.screen_mode);
        //if (kb.state.menu) lv_screen_load(ui.screen_menu);
        //if (kb.state.data) lv_screen_load(ui.screen_data);
        
        usleep(300000);
    }
    keyboard_deinit(&kb);
    return 0;
}
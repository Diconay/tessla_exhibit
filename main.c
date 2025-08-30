#include "lvgl/src/lvgl.h"
#include "lvgl/src/drivers/display/fb/lv_linux_fbdev.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "ui.h"
#include "keyboard.h"
#include <stdbool.h>

#define DEFAULT_FBDEV   "/dev/fb0"

#define OIL_CHANGE_INTERVAL   100
#define AIR_FILTER_INTERVAL   200
#define SPARK_PLUG_INTERVAL   300

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

static bool sim_running = false;
static int32_t gen_kw = 0;
static lv_value_precise_t grid_freq = 50.0f;

/* statistics counters */
static double runtime_hours = 0.0;
static double generation_kwh = 0.0;
static double oil_hours = 0.0;
static double air_filter_hours = 0.0;
static double plug_hours = 0.0;


int main(void) {
    lv_init();
    lv_linux_disp_init();

    srand(time(NULL));
    oil_hours       = rand() % OIL_CHANGE_INTERVAL;
    air_filter_hours= rand() % AIR_FILTER_INTERVAL;
    plug_hours      = rand() % SPARK_PLUG_INTERVAL;
    runtime_hours   = (oil_hours + air_filter_hours + plug_hours) * 5 + 2345;
    generation_kwh  = runtime_hours * 600.0;


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

    struct timespec ts_prev;
    clock_gettime(CLOCK_MONOTONIC, &ts_prev);

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

        struct timespec ts_now;
        clock_gettime(CLOCK_MONOTONIC, &ts_now);
        double elapsed_sec = (ts_now.tv_sec - ts_prev.tv_sec) +
                             (ts_now.tv_nsec - ts_prev.tv_nsec) / 1e9;
        ts_prev = ts_now;
        double sim_hours = elapsed_sec / 60.0; /* 1 real min -> 1 sim hour */

        if (sim_running) {
            runtime_hours += sim_hours;
            oil_hours += sim_hours;
            air_filter_hours += sim_hours;
            plug_hours += sim_hours;
            generation_kwh += gen_kw * sim_hours;

            if (oil_hours >= OIL_CHANGE_INTERVAL)
                oil_hours = 0;
            if (air_filter_hours >= AIR_FILTER_INTERVAL)
                air_filter_hours = 0;
            if (plug_hours >= SPARK_PLUG_INTERVAL)
                plug_hours = 0;
        }

        char buf[32];
        snprintf(buf, sizeof(buf), "%d ч", (int)oil_hours);
        ui_update_stat_oil(buf);
        snprintf(buf, sizeof(buf), "%d ч", (int)air_filter_hours);
        ui_update_stat_air(buf);
        snprintf(buf, sizeof(buf), "%d ч", (int)plug_hours);
        ui_update_stat_plug(buf);
        snprintf(buf, sizeof(buf), "%d ч", (int)runtime_hours);
        ui_update_stat_runtime(buf);
        snprintf(buf, sizeof(buf), "%.0f кВтч", generation_kwh);
        ui_update_stat_generation(buf);

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
#include "lvgl/src/lvgl.h"
#include "lvgl/src/drivers/display/fb/lv_linux_fbdev.h"
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include <cmath>

#include "ui.h"
#include "keyboard.h"

#define DEFAULT_FBDEV   "/dev/fb0"

#define OIL_CHANGE_INTERVAL   100
#define AIR_FILTER_INTERVAL   200
#define SPARK_PLUG_INTERVAL   300

#define GPU_STOPED  0
#define GPU_START   1
#define GPU_STOP    2
#define GPU_STABILIZATION    3
#define GPU_WARM    4
#define GPU_LANCH 5
#define GPU_WORK 6
#define GPU_POWER_DOWN 7
#define GPU_COOL 8
#define GPU_STOPING 9



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

static int32_t gpu_state;
static int32_t gen_kw = 0;
static lv_value_precise_t grid_freq = 50.0f;
static int32_t mains_p = 111;
static int32_t mains_q = 0;
static int32_t load = 0;
static lv_value_precise_t mains_cos = 0.99f;
static lv_value_precise_t mains_ua = 6.27f;
static lv_value_precise_t mains_ub = 6.16f;
static lv_value_precise_t mains_uc = 6.17f;

static double engine_rpm = 0.0;
static lv_value_precise_t oil_pressure;
static const double ENGINE_OIL_PRESSURE_MAX = 6.0; // bar
static const double ENGINE_OIL_PRESSURE_RPM_LIMIT = 1000.0; // rpm at which pressure stops increasing
static lv_value_precise_t gen_ua = 0.0f;
static lv_value_precise_t gen_ub = 0.0f;
static lv_value_precise_t gen_uc = 0.0f;
static lv_value_precise_t gen_freq = 0.0f;
static lv_value_precise_t gen_t = 17.2f;


/* statistics counters */
static double runtime_hours = 0.0;
static double generation_kwh = 0.0;
static double oil_hours = 0.0;
static double air_filter_hours = 0.0;
static double plug_hours = 0.0;
static int menu_tab = 0;
static int gen_params[3] = {0, 0, 0};
static int dvs_params[3] = {0, 0, 0};
static int menu_state = 0;
static int menu_param_idx = 0;

static int prev_value;


float generate_noise(float amplitude) {
    return (float)rand() / RAND_MAX * 2 * amplitude - amplitude;
}

int main(void) {
    lv_init();
    lv_linux_disp_init();

    srand(time(NULL));
    gpu_state = GPU_STOPED;
    oil_hours       = rand() % OIL_CHANGE_INTERVAL;
    air_filter_hours= rand() % AIR_FILTER_INTERVAL;
    plug_hours      = rand() % SPARK_PLUG_INTERVAL;
    runtime_hours   = (oil_hours + air_filter_hours + plug_hours) * 5 + 2345;
    generation_kwh  = runtime_hours * 600.0;


    pthread_t tick_th;
    pthread_create(&tick_th, NULL, tick_thread, NULL);

    UI ui;
    ui.init();

    Keyboard kb;
    if (!kb.init()) {
        // TODO print consol fpga open error
        return 1;
    }
    lv_screen_load(ui.screen_mode);

    struct timespec ts_prev;
    clock_gettime(CLOCK_MONOTONIC, &ts_prev);

    while (1)
    {
        lv_timer_handler();
        kb.poll();

        lv_value_precise_t delta = ((lv_value_precise_t)(rand() % 11) - 5.0f) / 1000.0f;
        grid_freq += delta;
        if (grid_freq > 50.05f)
            grid_freq = 50.05f;
        if (grid_freq < 49.95f)
            grid_freq = 49.95f;

        ui.update_power(gen_kw);
        ui.update_freq(grid_freq);
        
        int delta_p = (rand() % 11) - 5;
        mains_p += delta_p;
        if (mains_p > 1543)
            mains_p = 1543;
        if (mains_p < 398)
            mains_p = 398;

        lv_value_precise_t delta_cos = ((lv_value_precise_t)(rand() % 11) - 5.0f) / 1000.0f;
        mains_cos += delta_cos;
        if (mains_cos > 0.99f)
            mains_cos = 0.99f;
        if (mains_cos < 0.87f)
            mains_cos = 0.87f;

        lv_value_precise_t delta_ua = ((lv_value_precise_t)(rand() % 11) - 5.0f) / 100.0f;
        mains_ua += delta_ua;
        if (mains_ua > 6.3f)
            mains_ua = 6.3f;
        if (mains_ua < 6.0f)
            mains_ua = 6.0f;

        lv_value_precise_t delta_ub = ((lv_value_precise_t)(rand() % 11) - 5.0f) / 100.0f;
        mains_ub += delta_ub;
        if (mains_ub > 6.3f)
            mains_ub = 6.3f;
        if (mains_ub < 6.0f)
            mains_ub = 6.0f;

        lv_value_precise_t delta_uc = ((lv_value_precise_t)(rand() % 11) - 5.0f) / 100.0f;
        mains_uc += delta_uc;
        if (mains_uc > 6.3f)
            mains_uc = 6.3f;
        if (mains_uc < 6.0f)
            mains_uc = 6.0f;

        mains_q = mains_p * std::sqrt(1.0 / (mains_cos * mains_cos) - 1.0);

        if (kb.state.start) gpu_state = GPU_START;
        if (kb.state.stop) {
            if (gpu_state == GPU_LANCH || gpu_state == GPU_WORK) gpu_state = GPU_POWER_DOWN;
            else if (gpu_state == GPU_STABILIZATION || gpu_state == GPU_START || gpu_state == GPU_WARM) gpu_state = GPU_COOL;
        }

        switch (gpu_state) {
        case GPU_START:
            if (engine_rpm < 100) engine_rpm += 35;
            else if (engine_rpm < 200) engine_rpm += 34;
            else if (engine_rpm < 300) engine_rpm += 33;
            else if (engine_rpm < 400) engine_rpm += 32;
            else if (engine_rpm < 500) engine_rpm += 31;
            else if (engine_rpm < 600) engine_rpm += 30;
            else if (engine_rpm < 700) engine_rpm += 19;
            else if (engine_rpm < 800) engine_rpm += 18;
            else if (engine_rpm < 900) engine_rpm += 17;
            else if (engine_rpm < 1000) engine_rpm += 17;
            else if (engine_rpm < 1100) engine_rpm += 17;
            else if (engine_rpm < 1200) engine_rpm += 8;
            else if (engine_rpm < 1500) engine_rpm += 6;
            else gpu_state = GPU_STABILIZATION;
            break;
        case GPU_STABILIZATION:
            engine_rpm = 1500 + generate_noise(10.0);
            if (gen_ua < 6.1) gen_ua += 0.2;
            if (gen_ub < 6.1) gen_ub += 0.18;
            if (gen_uc < 6.1) gen_uc += 0.21;
            if (gen_ua > 6.1 && gen_ub > 6.1 && gen_uc > 6.1) gpu_state = GPU_WARM;
            break;
        case GPU_WARM:
            if (gen_t > 40.0) gpu_state = GPU_LANCH;
            engine_rpm = 1500 + generate_noise(10.0);
            gen_ua = 6.1 + generate_noise(0.1);
            gen_ub = 6.1 + generate_noise(0.1);
            gen_uc = 6.1 + generate_noise(0.1);
            break;
        case GPU_LANCH:
            engine_rpm = grid_freq / 50.0 * 1500.0;
            gen_ua = mains_ua;
            gen_ub = mains_ub;
            gen_uc = mains_uc;
            gen_kw += 3;
            if (gen_kw > 1000) gpu_state = GPU_WORK;
            break;
        case GPU_WORK:
            engine_rpm = grid_freq / 50.0 * 1500.0;
            gen_ua = mains_ua;
            gen_ub = mains_ub;
            gen_uc = mains_uc;
            engine_rpm = grid_freq / 50.0 * 1500.0;
            gen_kw = 1000 + (int)generate_noise(15);
            break;
        case GPU_POWER_DOWN:
            engine_rpm = grid_freq / 50.0 * 1500.0;
            gen_ua = mains_ua;
            gen_ub = mains_ub;
            gen_uc = mains_uc;
            gen_kw -= 5;
            if (gen_kw < 0) {
                gen_kw = 0;
                gpu_state = GPU_COOL;
            }
            break;
        case GPU_COOL:
            engine_rpm = 1500 + generate_noise(10.0);
            gen_ua -= 0.5;
            gen_ub -= 0.5;
            gen_uc -= 0.5;
            if (gen_ua < 0.0) gen_ua = 0.0;
            if (gen_ub < 0.0) gen_ub = 0.0;
            if (gen_uc < 0.0) gen_uc = 0.0;
            if (gen_t < 60.0) gpu_state = GPU_STOPING;
            break;
        case GPU_STOPING:
            engine_rpm -= 50;
            if (engine_rpm < 0) {
                engine_rpm = 0;
                gpu_state = GPU_STOP;
            }
            break;
        case GPU_STOP:
            break;
        }

        if (gpu_state != GPU_STOP && gpu_state != GPU_COOL && gpu_state != GPU_STOPING) {
            gen_t += 0.1;
            if (gen_kw > 1.0) gen_t += 0.2;
            if (gen_t > 85.0) gen_t = 85 + generate_noise(0.1);
        } else {
            gen_t -= 0.5;
            if (gen_t < 17.2) gen_t = 17.2;
        }
        ui.update_engine_coolant(gen_t);

        ui.update_gen_ua(gen_ua);
        ui.update_gen_ub(gen_ub);
        ui.update_gen_uc(gen_uc);

        struct timespec ts_now;
        clock_gettime(CLOCK_MONOTONIC, &ts_now);
        double elapsed_sec = (ts_now.tv_sec - ts_prev.tv_sec) +
                             (ts_now.tv_nsec - ts_prev.tv_nsec) / 1e9;
        ts_prev = ts_now;
        double sim_hours = elapsed_sec / 60.0;

        if (gpu_state != GPU_STOP) {
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
        
        ui.update_engine_rpm((int32_t)engine_rpm);
        gen_freq = engine_rpm / 1500.0 * 50.0;
        ui.update_gen_freq((float)gen_freq);

        oil_pressure = engine_rpm * ENGINE_OIL_PRESSURE_MAX / ENGINE_OIL_PRESSURE_RPM_LIMIT;
        if (oil_pressure > ENGINE_OIL_PRESSURE_MAX)
            oil_pressure = ENGINE_OIL_PRESSURE_MAX;
        ui.update_engine_oil((float)oil_pressure);

        char buf[32];
        snprintf(buf, sizeof(buf), "%d ч", (int)oil_hours);
        ui.update_stat_oil(buf);
        snprintf(buf, sizeof(buf), "%d ч", (int)air_filter_hours);
        ui.update_stat_air(buf);
        snprintf(buf, sizeof(buf), "%d ч", (int)plug_hours);
        ui.update_stat_plug(buf);
        snprintf(buf, sizeof(buf), "%d ч", (int)runtime_hours);
        ui.update_stat_runtime(buf);
        snprintf(buf, sizeof(buf), "%.0f кВтч", generation_kwh);
        ui.update_stat_generation(buf);

        load = mains_p - gen_kw;

        ui.update_power(gen_kw);
        ui.update_freq(grid_freq);
        ui.update_mains_p(load);
        ui.update_mains_q(mains_q);
        ui.update_mains_cos(mains_cos);
        ui.update_mains_ua(mains_ua);
        ui.update_mains_ub(mains_ub);
        ui.update_mains_uc(mains_uc);

        ui.update_gen_p(gen_kw);
        ui.update_gen_s(gen_kw / mains_cos);

        if (kb.state.mode) lv_screen_load(ui.screen_mode);
        if (kb.state.menu) {
            lv_screen_load(ui.screen_menu);
            menu_tab = 0;
            lv_tabview_set_act(ui.menu_tabview, menu_tab, LV_ANIM_OFF);
            menu_state = 0;
            menu_param_idx = 0;
        }
        if (lv_screen_active() == ui.screen_menu) {
            if (menu_state == 0) {
                if (kb.state.left && menu_tab > 0) {
                    menu_tab--;
                    lv_tabview_set_act(ui.menu_tabview, menu_tab, LV_ANIM_OFF);
                }
                if (kb.state.right && menu_tab < 1) {
                    menu_tab++;
                    lv_tabview_set_act(ui.menu_tabview, menu_tab, LV_ANIM_OFF);
                }
                if (kb.state.enter || kb.state.down) {
                    menu_state = 1;
                    menu_param_idx = 0;
                }
            } else if (menu_state == 1) {
                int max_params = 3;
                int *params = (menu_tab == 0) ? gen_params : dvs_params;
                if (kb.state.up && menu_param_idx == 0) menu_state = 0;
                if (kb.state.up && menu_param_idx > 0) menu_param_idx--;
                if (kb.state.down && menu_param_idx < max_params - 1) menu_param_idx++;
                if (kb.state.cancel) menu_state = 0;
                if (kb.state.enter) {
                    menu_state = 2;
                    prev_value = params[menu_param_idx];
                }
            } else if (menu_state == 2) {
                int *params = (menu_tab == 0) ? gen_params : dvs_params;
                if (kb.state.up) params[menu_param_idx]++;
                if (kb.state.left) params[menu_param_idx] += 10;
                if (kb.state.down && params[menu_param_idx] > 0) params[menu_param_idx]--;
                if (kb.state.right && params[menu_param_idx] > 10) params[menu_param_idx] -= 10;
                if (kb.state.enter) menu_state = 1;
                if (kb.state.cancel) {
                    menu_state = 1;
                    params[menu_param_idx] = prev_value;
                }
            }
            for (int i = 0; i < 3; i++) {
                lv_label_set_text_fmt(ui.gen_param_vals[i], "%d сек", gen_params[i]);
                lv_label_set_text_fmt(ui.dvs_param_vals[i], "%d сек", dvs_params[i]);
                lv_obj_set_style_bg_opa(ui.gen_param_rows[i], (menu_tab == 0 && menu_state > 0 && menu_param_idx == i) ? (menu_state == 2 ? LV_OPA_60 : LV_OPA_30) : LV_OPA_TRANSP, 0);
                lv_obj_set_style_bg_opa(ui.dvs_param_rows[i], (menu_tab == 1 && menu_state > 0 && menu_param_idx == i) ? (menu_state == 2 ? LV_OPA_60 : LV_OPA_30) : LV_OPA_TRANSP, 0);
            }
        }
        //if (kb.state.data) lv_screen_load(ui.screen_data);
        
        usleep(300000);
    }
    kb.deinit();
    return 0;
}
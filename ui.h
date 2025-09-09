#ifndef UI_H
#define UI_H

#include "lvgl/src/lvgl.h"

class UI {
public:
    UI();

    void init();

    void update_power(int32_t value);
    void update_freq(float value);
    void update_mains_p(int32_t value);
    void update_mains_q(int32_t value);
    void update_mains_cos(float value);
    void update_mains_ua(float value);
    void update_mains_ub(float value);
    void update_mains_uc(float value);

    void update_stat_oil(const char *value);
    void update_stat_air(const char *value);
    void update_stat_plug(const char *value);
    void update_stat_runtime(const char *value);
    void update_stat_generation(const char *value);

    void update_gen_p(int32_t value);
    void update_gen_s(int32_t value);
    void update_gen_freq(float value);
    void update_gen_ua(float value);
    void update_gen_ub(float value);
    void update_gen_uc(float value);

    void update_engine_oil(float value);
    void update_engine_coolant(float value);
    void update_engine_rpm(int32_t value);

    lv_obj_t *screen_mode;
    lv_obj_t *screen_menu;
    lv_obj_t *screen_data;
    lv_obj_t *menu_tabview;
    lv_obj_t *gen_param_vals[3];
    lv_obj_t *gen_param_rows[3];
    lv_obj_t *dvs_param_vals[3];
    lv_obj_t *dvs_param_rows[3];
};

#endif // UI_H
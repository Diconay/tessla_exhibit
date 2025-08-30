#ifndef UI_H
#define UI_H

#include "lvgl/src/lvgl.h"

struct ui {
    lv_obj_t *screen_mode;
    lv_obj_t *screen_menu;
    lv_obj_t *screen_data;
};

void ui_init(struct ui *ui);
void ui_update_power(int32_t value);
void ui_create(void);

void ui_update_freq(float value);
void ui_update_mains_p(int32_t value);
void ui_update_mains_q(int32_t value);
void ui_update_mains_cos(float value);
void ui_update_mains_ua(float value);
void ui_update_mains_ub(float value);
void ui_update_mains_uc(float value);

void ui_update_stat_oil(const char *value);
void ui_update_stat_air(const char *value);
void ui_update_stat_plug(const char *value);
void ui_update_stat_runtime(const char *value);
void ui_update_stat_generation(const char *value);

void ui_update_gen_p(int32_t value);
void ui_update_gen_s(int32_t value);
void ui_update_gen_freq(float value);
void ui_update_gen_ua(float value);
void ui_update_gen_ub(float value);
void ui_update_gen_uc(float value);

void ui_update_engine_oil(float value);
void ui_update_engine_coolant(float value);
void ui_update_engine_rpm(int32_t value);

#endif // UI_H
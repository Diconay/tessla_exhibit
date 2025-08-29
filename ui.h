#ifndef UI_H
#define UI_H

#include "lvgl/src/lvgl.h"

struct ui {
    lv_obj_t *screen_mode;
    lv_obj_t *screen_menu;
    lv_obj_t *screen_data;
};

void ui_init(struct ui *ui);

#endif // UI_H
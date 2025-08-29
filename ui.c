#include "ui.h"
#include "fonts/castom_fonts.h"

#define COLOR_GRAFIT   0x494d4e //0x3b3b48
#define COLOR_TEXT     0x8f8f8f

void ui_init(struct ui *ui)
{
    lv_obj_t *screen_mode = lv_screen_active();
    lv_obj_set_style_bg_color(screen_mode, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);
    lv_obj_t *label_mode = lv_label_create(screen_mode);
    lv_label_set_text(label_mode, "Главный экран");
    lv_obj_set_style_text_color(label_mode, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(label_mode, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(label_mode, &font_mulish_medium48, 0);

    lv_obj_t *screen_menu = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_menu, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);
    lv_obj_t *label_menu = lv_label_create(screen_menu);
    lv_label_set_text(label_menu, "Меню");
    lv_obj_set_style_text_color(label_menu, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(label_menu, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(label_menu, &font_mulish_medium48, 0);

    lv_obj_t *screen_data = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_data, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);
    lv_obj_t *label_data = lv_label_create(screen_data);
    lv_label_set_text(label_data, "Данные");
    lv_obj_set_style_text_color(label_data, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(label_data, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(label_data, &font_mulish_medium48, 0);

    ui->screen_mode = screen_mode;
    ui->screen_menu = screen_menu;
    ui->screen_data = screen_data;
}
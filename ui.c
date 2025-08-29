#include "ui.h"
#include "fonts/castom_fonts.h"

#define COLOR_GRAFIT   0x0B1E3A
#define COLOR_TEXT     0x8f8f8f

static lv_obj_t * needle_line;

static void set_needle_line_value(void * obj, int32_t v)
{
    lv_scale_set_line_needle_value((lv_obj_t *)obj, needle_line, 60, v);
}

void power_scale(lv_obj_t *screen){
    lv_obj_t * scale_line = lv_scale_create(screen);
    lv_obj_set_size(scale_line, 150, 150);
    lv_scale_set_mode(scale_line, LV_SCALE_MODE_ROUND_INNER);
    lv_obj_set_style_bg_opa(scale_line, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scale_line, lv_palette_lighten(LV_PALETTE_RED, 5), 0);
    lv_obj_set_style_radius(scale_line, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(scale_line, true, 0);
    lv_obj_align(scale_line, LV_ALIGN_LEFT_MID, LV_PCT(2), 0);

    lv_scale_set_label_show(scale_line, true);
    lv_scale_set_total_tick_count(scale_line, 31);
    lv_scale_set_major_tick_every(scale_line, 5);
    lv_obj_set_style_length(scale_line, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale_line, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale_line, 10, 40);

    lv_scale_set_angle_range(scale_line, 270);
    lv_scale_set_rotation(scale_line, 135);

    lv_obj_set_style_line_width(needle_line, 6, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(needle_line, true, LV_PART_MAIN);
    
    lv_anim_t anim_scale_line;
    lv_anim_init(&anim_scale_line);
    lv_anim_set_var(&anim_scale_line, scale_line);
    lv_anim_set_exec_cb(&anim_scale_line, set_needle_line_value);
    lv_anim_set_duration(&anim_scale_line, 1000);
    lv_anim_set_repeat_count(&anim_scale_line, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_reverse_duration(&anim_scale_line, 1000);
    lv_anim_set_values(&anim_scale_line, 10, 40);
    lv_anim_start(&anim_scale_line);
}

void ui_init(struct ui *ui)
{
    lv_obj_t *screen_mode = lv_screen_active();
    lv_obj_set_style_bg_color(screen_mode, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);
    lv_obj_t *label_mode = lv_label_create(screen_mode);
    lv_label_set_text(label_mode, "Главный экран");
    lv_obj_set_style_text_color(label_mode, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(label_mode, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(label_mode, &font_mulish_medium48, 0);

    power_scale(screen_mode);

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
#include "ui.h"
#include "fonts/castom_fonts.h"

#define COLOR_GRAFIT   0x0B1E3A
#define COLOR_TEXT     0xFFFFFF

static const int PAD = 10;
static const int R   = 10;
static const int STATUS_BAR_H = 60;

static lv_obj_t *scale_line;
static lv_obj_t *needle_line;
static lv_obj_t *power_label;

static lv_style_t st_screen, st_panel, st_title, st_row_lbl, st_row_val;

void style_init(void) {
    lv_style_init(&st_panel);
    lv_style_set_bg_color(&st_panel, lv_palette_main(COLOR_GRAFIT));
    lv_style_set_bg_opa(&st_panel, LV_OPA_COVER);
    lv_style_set_radius(&st_panel, R);
    lv_style_set_pad_all(&st_panel, PAD);

    lv_style_init(&st_title);
    lv_style_set_text_color(&st_title, lv_color_white());
    lv_style_set_text_letter_space(&st_title, 1);
    lv_style_set_text_font(&st_title, &font_mulish_medium36);

    lv_style_init(&st_row_lbl);
    lv_style_set_text_color(&st_row_lbl, lv_color_white());
    lv_style_set_text_font(&st_row_lbl, &font_mulish_medium24);

    lv_style_init(&st_row_val);
    lv_style_set_text_color(&st_row_val, lv_color_white());
    lv_style_set_text_font(&st_row_val, &font_mulish_medium24);
}

static lv_obj_t *make_panel(lv_obj_t *parent, const char *title) {
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(cont, LV_OPA_0, LV_PART_MAIN);

    lv_obj_set_style_border_color(cont, lv_color_white(), 0);
    lv_obj_set_style_border_opa(cont, LV_OPA_30, 0);
    lv_obj_set_style_border_width(cont, 2, 0);


    lv_obj_set_style_radius(cont, R, 0);
    lv_obj_set_style_pad_all(cont, PAD, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(cont, 2, 0);
    lv_obj_t *t = lv_label_create(cont);
    lv_obj_add_style(t, &st_title, 0);
    lv_label_set_text(t, title);
    return cont;
}

static void add_row(lv_obj_t *parent, const char *left, const char *right) {
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_remove_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_ver(row, 4, 0);
    lv_obj_set_layout(row, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_width(row, LV_PCT(100));
    lv_obj_set_height(row, 40);

    lv_obj_t *l = lv_label_create(row);
    lv_obj_add_style(l, &st_row_lbl, 0);
    lv_label_set_text(l, left);
    lv_obj_set_flex_grow(l, 1);

    lv_obj_t *v = lv_label_create(row);
    lv_obj_add_style(v, &st_row_val, 0);
    lv_label_set_text(v, right);
}

static lv_obj_t *add_status_bar(lv_obj_t *parent, const char *title) {
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_remove_flag(bar, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(bar, lv_color_hex(0x3C415D), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_opa(bar, LV_OPA_TRANSP, 0);
    lv_obj_set_size(bar, LV_PCT(100), STATUS_BAR_H);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *lbl = lv_label_create(bar);
    lv_label_set_text(lbl, title);
    lv_obj_set_style_text_color(lbl, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_set_style_text_font(lbl, &font_mulish_medium48, LV_PART_MAIN);
    lv_obj_center(lbl);
    return bar;
}

void power_scale(lv_obj_t *screen){
    scale_line = lv_scale_create(screen);
    lv_obj_set_size(scale_line, 230, 230);
    lv_scale_set_mode(scale_line, LV_SCALE_MODE_ROUND_OUTER);
    lv_obj_set_style_bg_opa(scale_line, LV_OPA_TRANSP, 0);
    lv_obj_align(scale_line, LV_ALIGN_CENTER, LV_PCT(2), 0);

    lv_scale_set_label_show(scale_line, true);
    lv_obj_set_style_text_color(scale_line, lv_color_white(), LV_PART_INDICATOR);
    lv_obj_set_style_text_font(scale_line, &lv_font_montserrat_20, 0);
    lv_obj_set_style_line_color(scale_line, lv_color_white(), LV_PART_ITEMS);
    lv_obj_set_style_line_color(scale_line, lv_color_white(), LV_PART_INDICATOR);
    lv_scale_set_total_tick_count(scale_line, 31);
    lv_scale_set_major_tick_every(scale_line, 6);
    lv_obj_set_style_length(scale_line, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale_line, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale_line, 0, 1250);

    lv_scale_set_angle_range(scale_line, 225);
    lv_scale_set_rotation(scale_line, 180);

    lv_obj_set_style_arc_width(scale_line, 6, LV_PART_MAIN);
    lv_obj_set_style_arc_color(scale_line, lv_color_white(), LV_PART_MAIN);

    lv_obj_t *seg_green = lv_arc_create(scale_line);
    lv_obj_remove_style_all(seg_green);
    lv_obj_set_size(seg_green, 200, 200);
    lv_arc_set_bg_angles(seg_green, 180, 300);
    lv_obj_set_style_arc_width(seg_green, 40, 0);
    lv_obj_set_style_arc_color(seg_green, lv_palette_main(LV_PALETTE_GREEN), 0);
    lv_obj_center(seg_green);

    lv_obj_t *seg_yellow = lv_arc_create(scale_line);
    lv_obj_remove_style_all(seg_yellow);
    lv_obj_set_size(seg_yellow, 200, 200);
    lv_arc_set_bg_angles(seg_yellow, 300, 0);
    lv_obj_set_style_arc_width(seg_yellow, 40, 0);
    lv_obj_set_style_arc_color(seg_yellow, lv_palette_main(LV_PALETTE_YELLOW), 0);
    lv_obj_center(seg_yellow);

    lv_obj_t *seg_red = lv_arc_create(scale_line);
    lv_obj_remove_style_all(seg_red);
    lv_obj_set_size(seg_red, 200, 200);
    lv_arc_set_bg_angles(seg_red, 0, 45);
    lv_obj_set_style_arc_width(seg_red, 40, 0);
    lv_obj_set_style_arc_color(seg_red, lv_palette_main(LV_PALETTE_RED), 0);
    lv_obj_center(seg_red);

    needle_line = lv_line_create(scale_line);
    lv_obj_set_style_line_width(needle_line, 6, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(needle_line, true, LV_PART_MAIN);
    lv_obj_set_style_line_color(needle_line, lv_color_white(), LV_PART_MAIN);
    
    power_label = lv_label_create(screen);
    lv_obj_set_size(power_label, 200, LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(power_label, lv_color_white(), 0);
    lv_obj_align_to(power_label, scale_line, LV_ALIGN_CENTER, -80, 35);
    lv_obj_set_style_text_font(power_label, &font_mulish_medium36, 0);
    lv_obj_set_style_text_align(power_label, LV_TEXT_ALIGN_RIGHT, 0);
}

void ui_update_power(int32_t value)
{
    lv_scale_set_line_needle_value(scale_line, needle_line, 102, value);
    lv_label_set_text_fmt(power_label, "%d кВт", value);
}

void ui_init(struct ui *ui)
{
    lv_obj_t *screen_mode = lv_screen_active();
    lv_obj_set_style_bg_color(screen_mode, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);
    

    style_init();

    static int32_t col_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };
    static int32_t row_dsc[] = { LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST };

    lv_display_t *disp = lv_obj_get_display(screen_mode);
    lv_coord_t ver_res = lv_disp_get_ver_res(disp);

    lv_obj_t *root = lv_obj_create(screen_mode);
    lv_obj_remove_flag(root, LV_OBJ_FLAG_SCROLLABLE);   
    lv_obj_set_style_bg_opa(root, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(root, LV_OPA_0, 0);
    lv_obj_set_grid_dsc_array(root, col_dsc, row_dsc);
    lv_obj_set_size(root, LV_PCT(100), ver_res - STATUS_BAR_H);
    lv_obj_align(root, LV_ALIGN_TOP_MID, 0, STATUS_BAR_H);

    lv_obj_t *mains_panel = make_panel(root, "Сеть");
    lv_obj_set_grid_cell(mains_panel, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    add_row(mains_panel, "P сети:", "111 кВт");
    add_row(mains_panel, "Q сети:", "6 кВАр");
    add_row(mains_panel, "cos:", "0.99");
    add_row(mains_panel, "F сети:", "50.01 Гц");
    add_row(mains_panel, "Ua:", "6.27 кВ");
    add_row(mains_panel, "Uб:", "6.16 кВ");
    add_row(mains_panel, "Uc:", "6.17 кВ");

    lv_obj_t *stat_panel = make_panel(root, "Статистика");
    lv_obj_set_grid_cell(stat_panel, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    add_row(stat_panel, "Масло:", "—");
    add_row(stat_panel, "Возд. фильтр:", "—");
    add_row(stat_panel, "Свечи:", "—");
    add_row(stat_panel, "Наработка:", "—");
    add_row(stat_panel, "Выработка:", "—");

    lv_obj_t *power_panel = make_panel(root, "Мощность");
    lv_obj_set_grid_cell(power_panel, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_flex_align(power_panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    power_scale(power_panel);

    lv_obj_t *gen_panel = make_panel(root, "Генератор");
    lv_obj_set_grid_cell(gen_panel, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    add_row(gen_panel, "P:", "—");
    add_row(gen_panel, "S:", "—");
    add_row(gen_panel, "F:", "—");
    add_row(gen_panel, "Ua:", "—");
    add_row(gen_panel, "Ub:", "—");
    add_row(gen_panel, "Uc:", "—");

    lv_obj_t *engine_panel = make_panel(root, "Двигатель");
    lv_obj_set_grid_cell(engine_panel, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    add_row(engine_panel, "Давл. масла:", "—");
    add_row(engine_panel, "Темп. охл. жид:", "—");
    add_row(engine_panel, "Обороты:", "—");

    add_status_bar(screen_mode, "Управление ГПУ");

    lv_obj_t *screen_menu = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_menu, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);

    lv_obj_t *screen_data = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_data, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);

    ui->screen_mode = screen_mode;
    ui->screen_menu = screen_menu;
    ui->screen_data = screen_data;
}
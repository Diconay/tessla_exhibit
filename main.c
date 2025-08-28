#include "lvgl/src/lvgl.h"
#include "lvgl/src/drivers/display/fb/lv_linux_fbdev.h"

#define DEFAULT_FBDEV   "/dev/fb0"

#define COLOR_GRAFIT1   0x494d4e
#define COLOR_GRAFIT2   0x3b3b48
#define COLOR_TEXT      0x8f8f8f

static void lv_linux_disp_init(void){
    lv_display_t * disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, DEFAULT_FBDEV);
}

int main(void) {
    lv_init();
    lv_linux_disp_init();

    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(COLOR_GRAFIT1), LV_PART_MAIN);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Main");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);

    while (1)
    {
        lv_timer_handler();
    }
}
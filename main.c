#include "lvgl/src/lvgl.h"
#include "lvgl/src/drivers/display/fb/lv_linux_fbdev.h"
#include <pthread.h>
#include <unistd.h>

#include "keyboard.h"

#define COLOR_GRAFIT   0x494d4e //0x3b3b48
#define COLOR_TEXT     0x8f8f8f

#define DEFAULT_FBDEV   "/dev/fb0"

static void lv_linux_disp_init(void){
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

int main(void) {
    lv_init();
    lv_linux_disp_init();

    pthread_t tick_th;
    pthread_create(&tick_th, NULL, tick_thread, NULL);

    /* Mode screen */
    lv_obj_t *screen_mode = lv_screen_active();
    lv_obj_set_style_bg_color(screen_mode, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);
    lv_obj_t *label_mode = lv_label_create(screen_mode);
    lv_label_set_text(label_mode, "Main screen");
    lv_obj_set_style_text_color(label_mode, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(label_mode, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_text_font(label_mode, &lv_font_montserrat_30, 0);

    /* Menu screen */
    lv_obj_t *screen_menu = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_menu, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);
    lv_obj_t *label_menu = lv_label_create(screen_menu);
    lv_label_set_text(label_menu, "Menu");
    lv_obj_set_style_text_color(label_menu, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(label_menu, LV_ALIGN_TOP_MID, 0, 0);

    // Data screen
    lv_obj_t *screen_data = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen_data, lv_color_hex(COLOR_GRAFIT), LV_PART_MAIN);
    lv_obj_t *label_data = lv_label_create(screen_data);
    lv_label_set_text(label_data, "data screen");
    lv_obj_set_style_text_color(label_data, lv_color_hex(COLOR_TEXT), LV_PART_MAIN);
    lv_obj_align(label_data, LV_ALIGN_TOP_MID, 0, 0);

    struct keyboard kb;
    if (keyboard_init(&kb) != 0) {
        lv_label_set_text(label_mode, "fpga open error");
        return 1;
    }
    
    while (1)
    {
        lv_timer_handler();
        keyboard_poll(&kb);
        if (kb.state.mode) lv_screen_load(screen_mode);
        if (kb.state.menu) lv_screen_load(screen_menu);
        if (kb.state.data) lv_screen_load(screen_data);
        usleep(5000);
    }
    keyboard_deinit(&kb);
    return 0;
}
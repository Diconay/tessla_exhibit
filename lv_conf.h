#ifndef LV_CONF_H
#define LV_CONF_H



/* Говорим LVGL, что конфиг лежит рядом и его нужно подключать просто как "lv_conf.h" */
#define LV_CONF_INCLUDE_SIMPLE 1

#define LV_USE_LED 1

/* Базовые вещи */
#define LV_USE_LOG               1
#define LV_LOG_LEVEL             LV_LOG_LEVEL_WARN
#define LV_USE_OS                LV_OS_NONE

/* Рендерер по умолчанию — софт (CPU) */
#define LV_USE_DRAW_SW           1

/* Изображения и их кэш (чтобы не было undefined refs) */
#define LV_USE_IMAGE             1
#define LV_USE_IMAGE_CACHE       1
#define LV_IMAGE_CACHE_DEF_SIZE  8
#define LV_USE_IMAGE_HEADER_CACHE 1

/* FBDEV-драйвер дисплея */
#define LV_USE_LINUX_FBDEV 1

/* Память (можно увеличить при мерцаниях) */
#define LV_MEM_CUSTOM            0
#define LV_MEM_SIZE              (64U * 1024U)

/* Шрифт по умолчанию */
#define LV_FONT_DEFAULT          &lv_font_montserrat_16
#define LV_FONT_MONTSERRAT_26 1

/* Чуть быстрее, без дорогих проверок */
#define LV_USE_ASSERT_OBJ              0
#define LV_USE_ASSERT_MEM_INTEGRITY    0
#define LV_USE_ASSERT_STYLE            0

#endif /* LV_CONF_H */

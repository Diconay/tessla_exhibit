# ---- настройки ----
CC      = g++
CFLAGS  = -O2 -Wall -I. -Ilvgl -Ilvgl/src -D_DEFAULT_SOURCE -DLV_CONF_INCLUDE_SIMPLE
LDFLAGS = -lpthread -lm
OBJDIR  = build
TARGET  = build/TESSla

# наше приложение
APP_SRCS = main.cpp keyboard.c ui.cpp fonts/mulish.c fonts/mulish_medium48.c fonts/mulish_medium36.c fonts/mulish_medium24.c

# поиск FBDEV-драйвера в типичных местах (v9 и др.)
FBDEV_SRC := $(firstword \
  $(wildcard lvgl/src/drivers/display/fb/lv_linux_fbdev.c) \
  $(wildcard lvgl/src/drivers/display/linux/lv_linux_fbdev.c) \
  $(wildcard lvgl/src/drivers/display/fbdev.c) \
)

ifeq ($(FBDEV_SRC),)
$(error Не найден FBDEV-драйвер LVGL. Проверьте версию LVGL (для v9: lvgl/src/drivers/display/fbdev/lv_fbdev.c))
endif

# ядро LVGL (без drivers/*)
ALL_LVGL_SRCS := \
  $(wildcard lvgl/src/*.c) \
  $(wildcard lvgl/src/*/*.c) \
  $(wildcard lvgl/src/*/*/*.c) \
  $(wildcard lvgl/src/*/*/*/*.c)

DRIVERS_SRCS := \
  $(wildcard lvgl/src/drivers/*.c) \
  $(wildcard lvgl/src/drivers/*/*.c) \
  $(wildcard lvgl/src/drivers/*/*/*.c) \
  $(wildcard lvgl/src/drivers/*/*/*/*.c)

CORE_SRCS := $(filter-out $(DRIVERS_SRCS),$(ALL_LVGL_SRCS))

SRCS = $(APP_SRCS) $(CORE_SRCS) $(FBDEV_SRC)
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(filter %.c,$(SRCS))) \
       $(patsubst %.cpp,$(OBJDIR)/%.o,$(filter %.cpp,$(SRCS)))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

# компиляция в build/ с автосозданием подкаталогов
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: all clean

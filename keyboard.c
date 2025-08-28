#include "keyboard.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define FPGA_DEVNODE "/dev/fpga_ctrl0"

struct fpga_reg_io { uint32_t offset; uint32_t value; };
#define FPGA_IOC_MAGIC   'f'
#define FPGA_IOC_READ32  _IOWR(FPGA_IOC_MAGIC, 0x00, struct fpga_reg_io)

static const uint32_t reg_offsets[] = {0x34, 0x38};

int keyboard_init(struct keyboard *kb)
{
    memset(kb, 0, sizeof(*kb));
    kb->fd = open(FPGA_DEVNODE, O_RDONLY);
    return kb->fd < 0 ? -1 : 0;
}

void keyboard_deinit(struct keyboard *kb)
{
    if (kb->fd >= 0) {
        close(kb->fd);
        kb->fd = -1;
    }
}

bool keyboard_poll(struct keyboard *kb)
{
    if (kb->fd < 0)
        return false;

    struct fpga_reg_io io;
    uint32_t regs[2] = {0};
    for (unsigned i = 0; i < 2; i++) {
        io.offset = reg_offsets[i];
        io.value = 0;
        if (ioctl(kb->fd, FPGA_IOC_READ32, &io) == 0)
            regs[i] = io.value;
    }

    bool changed = false;
    uint8_t *fields[] = {
        &kb->counts.start, &kb->counts.mode, &kb->counts.cancel, &kb->counts.left,
        &kb->counts.stop, &kb->counts.menu, &kb->counts.up, &kb->counts.down,
        &kb->counts.reset, &kb->counts.data, &kb->counts.enter, &kb->counts.right
    };

    uint8_t *states[] = {
        &kb->state.start, &kb->state.mode, &kb->state.cancel, &kb->state.left,
        &kb->state.stop, &kb->state.menu, &kb->state.up, &kb->state.down,
        &kb->state.reset, &kb->state.data, &kb->state.enter, &kb->state.right
    };

    for (unsigned i = 0; i < 12; i++) {
        unsigned reg_idx = i / 8;
        unsigned shift = (i % 8) * 4;
        uint8_t cnt = (regs[reg_idx] >> shift) & 0xF;
        *states[i] = 0;
        if (cnt != *fields[i]) {
            *states[i] = 1;
            *fields[i] = cnt;
            changed = true;
        }
    }

    return changed;
}
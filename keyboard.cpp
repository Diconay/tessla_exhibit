#include "keyboard.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstring>

#define FPGA_DEVNODE "/dev/fpga_ctrl0"

struct fpga_reg_io { uint32_t offset; uint32_t value; };
#define FPGA_IOC_MAGIC   'f'
#define FPGA_IOC_READ32  _IOWR(FPGA_IOC_MAGIC, 0x00, struct fpga_reg_io)

static const uint32_t reg_offsets[] = {0x34, 0x38};


Keyboard::Keyboard() : fd(-1) {
    std::memset(&counts, 0, sizeof(counts));
    std::memset(&state, 0, sizeof(state));
}

Keyboard::~Keyboard() {
    deinit();
}

bool Keyboard::init() {
    fd = open(FPGA_DEVNODE, O_RDONLY);
    return read(counts, state, true);
}

void Keyboard::deinit() {
    if (fd >= 0) {
        close(fd);
        fd = -1;
    }
}

bool Keyboard::poll() {
    if (!read(counts, state)) {
        return false;
    }

    return true;
}

bool Keyboard::read(State &cnt, State &st, bool init) {
    if (fd < 0)
        return false;

    struct fpga_reg_io io;
    uint32_t regs[2] = {0};
    for (unsigned i = 0; i < 2; i++) {
        io.offset = reg_offsets[i];
        io.value = 0;
        if (ioctl(fd, FPGA_IOC_READ32, &io) == 0)
            regs[i] = io.value;
    }

    uint8_t *fields[] = {
        &cnt.start, &cnt.mode, &cnt.cancel, &cnt.left,
        &cnt.stop, &cnt.menu, &cnt.up, &cnt.down,
        &cnt.reset, &cnt.data, &cnt.enter, &cnt.right
    };

    uint8_t *states[] = {
        &st.start, &st.mode, &st.cancel, &st.left,
        &st.stop, &st.menu, &st.up, &st.down,
        &st.reset, &st.data, &st.enter, &st.right
    };

    for (unsigned i = 0; i < 12; i++) {
        unsigned reg_idx = i / 8;
        unsigned shift = (i % 8) * 4;
        uint8_t c = (regs[reg_idx] >> shift) & 0xF;
        *states[i] = 0;
        if (!init && c != *fields[i]) *states[i] = 1;
        *fields[i] = c;
    }

    return true;
}
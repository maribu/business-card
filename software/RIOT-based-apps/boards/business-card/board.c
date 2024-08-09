#include <stdint.h>

#include "board.h"

struct ghost {
    uint8_t inputs;
    uint8_t ghost;
};

static const struct ghost ghosts[] = {
    {
        .inputs = BUTTON_UP | BUTTON_RIGHT,
        .ghost = BUTTON_LEFT,
    },
    {
        .inputs = BUTTON_DOWN | BUTTON_B,
        .ghost = BUTTON_A,
    },
    {
        .inputs = BUTTON_LEFT | BUTTON_DOWN,
        .ghost = BUTTON_UP,
    },
    {
        .inputs = BUTTON_RIGHT | BUTTON_A,
        .ghost = BUTTON_B,
    },
    {
        .inputs = BUTTON_A | BUTTON_UP,
        .ghost = BUTTON_DOWN,
    },
    {
        .inputs = BUTTON_B | BUTTON_LEFT,
        .ghost = BUTTON_RIGHT,
    },
};

uint8_t compensate_rollover(uint8_t scan)
{
    for (unsigned i = 0; i < ARRAY_SIZE(ghosts); i++) {
        if ((scan & ghosts[i].inputs) == ghosts[i].inputs) {
            scan &= ~(ghosts[i].ghost);
        }
    }

    return scan;
}

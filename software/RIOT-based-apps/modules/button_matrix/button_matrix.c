#include <string.h>

#include "button_matrix.h"
#include "button_matrix_params.h"
#include "periph/gpio_ll.h"

static uword_t button_io_masks[BUTTON_MATRIX_PIN_NUMOF];
static uword_t button_dir_masks[BUTTON_MATRIX_PIN_NUMOF];
static uword_t button_io_mask_all;
static uword_t button_dir_mask_all;

int button_matrix_init(void)
{
    int retval;

    for (unsigned i = 0; i < BUTTON_MATRIX_PIN_NUMOF; i++) {
        uword_t mask = 1U << button_matrix_pins[i];
        button_io_mask_all |= mask;
        button_dir_masks[i] = gpio_ll_prepare_switch_dir(mask);
        button_io_masks[i] = mask;
        gpio_conf_t conf = {
            .state = GPIO_INPUT,
            .pull = GPIO_PULL_DOWN,
            .slew_rate = GPIO_SLEW_FASTEST,
        };
        retval = gpio_ll_init(BUTTON_MATRIX_PORT, button_matrix_pins[i], conf);
        if (retval != 0) {
            return retval;
        }
    }

    button_dir_mask_all = gpio_ll_prepare_switch_dir(button_io_mask_all);

    return 0;
}

bool button_matrix_test(uint8_t _x, uint8_t y)
{
    uint8_t x = BUTTON_MATRIX_WIDTH - 1 - _x;
    if ((x == y) || (_x >= BUTTON_MATRIX_WIDTH) || (y >= BUTTON_MATRIX_HEIGHT)) {
        return false;
    }

    /* push everything low fast, pull downs won't be fast enough */
    gpio_ll_clear(BUTTON_MATRIX_PORT, button_io_mask_all);
    gpio_ll_switch_dir_output(BUTTON_MATRIX_PORT, button_dir_mask_all);
    /* go back to input */
    gpio_ll_switch_dir_input(BUTTON_MATRIX_PORT, button_dir_mask_all);

    gpio_ll_switch_dir_output(BUTTON_MATRIX_PORT, button_dir_masks[y]);
    gpio_ll_set(BUTTON_MATRIX_PORT, button_io_masks[y]);

    /* give the input buffer some time to register the change */
    for (unsigned i = 0; i < 8; i++) {
        (void)gpio_ll_read(BUTTON_MATRIX_PORT);
    }

    /* check if the button is pressed */
    bool result = (gpio_ll_read(BUTTON_MATRIX_PORT) & button_io_masks[x]);

    gpio_ll_switch_dir_input(BUTTON_MATRIX_PORT, button_dir_mask_all);

    return result;
}

void button_matrix_scan(uint8_t *dest)
{
    memset(dest, 0, (BUTTON_MATRIX_BUTTON_NUMOF + 7) / 8);
    size_t pos = 0;

    for (unsigned y = 0; y < BUTTON_MATRIX_HEIGHT; y++) {
        /* drive everything low quickly, pull down resistors may be weak */
        gpio_ll_clear(BUTTON_MATRIX_PORT, button_io_mask_all);
        gpio_ll_switch_dir_output(BUTTON_MATRIX_PORT, button_dir_mask_all);

        /* Switch all but column to input */
        gpio_ll_switch_dir_input(BUTTON_MATRIX_PORT, button_dir_mask_all ^ button_dir_masks[y]);

        /* now, drive the column high */
        gpio_ll_set(BUTTON_MATRIX_PORT, button_io_masks[y]);

        /* give the input buffer some time to register the change on the input pins */
        for (unsigned i = 0; i < 8; i++) {
            (void)gpio_ll_read(BUTTON_MATRIX_PORT);
        }
        uword_t col = gpio_ll_read(BUTTON_MATRIX_PORT);

        for (unsigned x = BUTTON_MATRIX_WIDTH - 1; x < BUTTON_MATRIX_WIDTH; x--) {
            if (x == y) {
                continue;
            }
            if (col & button_io_masks[x]) {
                dest[pos >> 3] |= (1U << (pos & 0x7));
            }
            pos++;
        }
    }

    gpio_ll_switch_dir_input(BUTTON_MATRIX_PORT, button_dir_mask_all);
}

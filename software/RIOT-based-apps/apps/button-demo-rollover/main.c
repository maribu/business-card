#include <assert.h>

#include "board.h"
#include "led_matrix.h"
#include "button_matrix.h"

static void update_dispaly(uint8_t scan, uint8_t y_offset)
{
    uint8_t bright;

    bright = (scan & BUTTON_UP) ? LED_MATRIX_BRIGHTNESS_MAX : 2;
    led_matrix_fb_set(1, 1 + y_offset, bright);

    bright = (scan & BUTTON_DOWN) ? LED_MATRIX_BRIGHTNESS_MAX : 2;
    led_matrix_fb_set(1, 3 + y_offset, bright);

    bright = (scan & BUTTON_LEFT) ? LED_MATRIX_BRIGHTNESS_MAX : 2;
    led_matrix_fb_set(0, 2 + y_offset, bright);

    bright = (scan & BUTTON_RIGHT) ? LED_MATRIX_BRIGHTNESS_MAX : 2;
    led_matrix_fb_set(2, 2 + y_offset, bright);

    bright = (scan & BUTTON_A) ? LED_MATRIX_BRIGHTNESS_MAX : 2;
    led_matrix_fb_set(6, 2 + y_offset, bright);

    bright = (scan & BUTTON_B) ? LED_MATRIX_BRIGHTNESS_MAX : 2;
    led_matrix_fb_set(4, 3 + y_offset, bright);
}

int main(void)
{
    int retval;

    retval = led_matrix_init();
    assert(retval == 0);

    retval = button_matrix_init();
    assert(retval == 0);
    (void)retval;

    uint32_t target_frame = led_matrix_frame_number();

    uint8_t scan_corrected = 0;
    uint8_t scan = 0;

    while (1) {
        button_matrix_scan(&scan);
        scan_corrected = compensate_rollover(scan);

        update_dispaly(scan, 0);
        update_dispaly(scan_corrected, 4);

        target_frame = led_matrix_fb_switch(target_frame);
    }

    return 0;
}

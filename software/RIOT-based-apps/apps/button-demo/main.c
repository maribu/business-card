#include <assert.h>

#include "board.h"
#include "led_matrix.h"
#include "button_matrix.h"

int main(void)
{
    int retval;

    retval = led_matrix_init();
    assert(retval == 0);

    retval = button_matrix_init();
    assert(retval == 0);
    (void)retval;

    uint32_t target_frame = led_matrix_frame_number();

    while (1) {
        led_matrix_fb_clear();

        if (button_matrix_test(BUTTON_MATRIX_COORD_UP)) {
            led_matrix_fb_set(2, 0, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(3, 0, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(2, 1, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(3, 1, LED_MATRIX_BRIGHTNESS_MAX);
        }

        if (button_matrix_test(BUTTON_MATRIX_COORD_DOWN)) {
            led_matrix_fb_set(2, 4, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(3, 4, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(2, 5, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(3, 5, LED_MATRIX_BRIGHTNESS_MAX);
        }

        if (button_matrix_test(BUTTON_MATRIX_COORD_LEFT)) {
            led_matrix_fb_set(0, 2, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(1, 2, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(0, 3, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(1, 3, LED_MATRIX_BRIGHTNESS_MAX);
        }

        if (button_matrix_test(BUTTON_MATRIX_COORD_RIGHT)) {
            led_matrix_fb_set(4, 2, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(5, 2, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(4, 3, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(5, 3, LED_MATRIX_BRIGHTNESS_MAX);
        }

        if (button_matrix_test(BUTTON_MATRIX_COORD_A)) {
            led_matrix_fb_set(8, 6, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(9, 6, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(8, 7, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(9, 7, LED_MATRIX_BRIGHTNESS_MAX);
        }

        if (button_matrix_test(BUTTON_MATRIX_COORD_B)) {
            led_matrix_fb_set(5, 7, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(6, 7, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(5, 8, LED_MATRIX_BRIGHTNESS_MAX);
            led_matrix_fb_set(6, 8, LED_MATRIX_BRIGHTNESS_MAX);
        }

        target_frame = led_matrix_fb_switch(target_frame);
    }

    return 0;
}

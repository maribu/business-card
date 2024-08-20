#include <assert.h>
#include <string.h>

#include "board.h"
#include "button_matrix.h"
#include "fmt.h"
#include "led_matrix.h"
#include "led_matrix_params.h"

#define GLYPH_FRAMES        60
#define GLYPH_MIN_FRAMES    10
#define BLANK_FRAMES        30
#define BLINK_FRAMES        5
#define BLINK_LOOPS         6

static const bitmap_glyph_t * const arrows[] = {
    &bitmap_glyph_arrow_up,
    &bitmap_glyph_arrow_down,
    &bitmap_glyph_arrow_left,
    &bitmap_glyph_arrow_right,
};

static uint32_t xorshift32(uint32_t x)
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

static const uint8_t btns_any[] = {
    BUTTON_A | BUTTON_B | BUTTON_UP | BUTTON_DOWN | BUTTON_LEFT | BUTTON_RIGHT
};

static bitmap_glyph_t simon_next_glyph(uint32_t *rnd_state)
{
    size_t idx;
    do {
        *rnd_state = xorshift32(*rnd_state);
        idx = *rnd_state & 0x7;
    } while (idx >= 6);

    if (idx < 4) {
        return *arrows[idx];
    }

    if (idx == 4) {
        return bitmap_font_get(&bitmap_font_matrix_light8, 'A');
    }

    return bitmap_font_get(&bitmap_font_matrix_light8, 'B');
}

static uint8_t simon_next_btn(uint32_t *rnd_state)
{
    size_t idx;
    do {
        *rnd_state = xorshift32(*rnd_state);
        idx = *rnd_state & 0x7;
    } while (idx >= 6);

    switch (idx) {
    default:
    case 0:
        return BUTTON_UP;
    case 1:
        return BUTTON_DOWN;
    case 2:
        return BUTTON_LEFT;
    case 3:
        return BUTTON_RIGHT;
    case 4:
        return BUTTON_A;
    case 5:
        return BUTTON_B;
    }
}

static bitmap_glyph_t glyph_by_key(uint8_t key)
{
    switch (key) {
    case BUTTON_UP:
        return *arrows[0];
    case BUTTON_DOWN:
        return *arrows[1];
    case BUTTON_LEFT:
        return *arrows[2];
    case BUTTON_RIGHT:
        return *arrows[3];
    case BUTTON_A:
        return bitmap_font_get(&bitmap_font_matrix_light8, 'A');
    case BUTTON_B:
        return bitmap_font_get(&bitmap_font_matrix_light8, 'B');
    default:
        return bitmap_font_get(&bitmap_font_matrix_light8, '?');
    }
}

int main(void)
{
    int retval;

    retval = led_matrix_init();
    assert(retval == 0);

    retval = button_matrix_init();
    assert(retval == 0);
    (void)retval;

    static const char *msg = "LED-mon says: Memorize the button sequence shown and enter it. Press any button to start";
    uint8_t btns_pressed[1];
    led_matrix_text_scroll_until_button(&bitmap_font_matrix_light8, msg, strlen(msg),
                                        btns_any, btns_pressed,
                                        sizeof(btns_pressed),
                                        LED_MATRIX_BRIGHTNESS_MAX);

    while (1) {
        uint32_t target_frame;
        uint32_t seed;
        uint32_t sequence_length;

game_restart:
        target_frame = led_matrix_frame_number();
        seed = target_frame ^ 0x55555555;
        sequence_length = 0;

        while (1) {
            sequence_length++;
            uint32_t rnd_state = seed;
            for (uint32_t i = 0; i < sequence_length; i++) {
                bitmap_glyph_t glyph = simon_next_glyph(&rnd_state);
                led_matrix_fb_clear();
                led_matrix_glyph(&glyph,
                                 (LED_MATRIX_WIDTH - glyph.width) / 2, (LED_MATRIX_HEIGHT - 8) / 2,
                                 LED_MATRIX_BRIGHTNESS_MAX);

                target_frame = led_matrix_fb_switch(target_frame) + GLYPH_FRAMES;
                led_matrix_fb_clear();
                target_frame = led_matrix_fb_switch(target_frame) + BLANK_FRAMES;
            }

            rnd_state = seed;
            for (uint32_t i = 0; i < sequence_length; i++) {
                do {
                    button_matrix_scan(btns_pressed);
                } while (btns_pressed[0] == 0);

                led_matrix_fb_clear();
                bitmap_glyph_t glyph = glyph_by_key(btns_pressed[0]);
                uint8_t btn_correct = simon_next_btn(&rnd_state);
                if (btns_pressed[0] != btn_correct) {
                    for (unsigned loop = 0; loop < BLINK_LOOPS; loop++) {
                        led_matrix_glyph(&glyph,
                                         (LED_MATRIX_WIDTH - glyph.width) / 2, (LED_MATRIX_HEIGHT - 8) / 2,
                                         LED_MATRIX_BRIGHTNESS_MAX);
                        target_frame = led_matrix_fb_switch(target_frame) + BLINK_FRAMES;
                        led_matrix_glyph(&glyph,
                                         (LED_MATRIX_WIDTH - glyph.width) / 2, (LED_MATRIX_HEIGHT - 8) / 2,
                                         1);
                        target_frame = led_matrix_fb_switch(target_frame) + BLINK_FRAMES;
                    }
                    led_matrix_fb_clear();
                    led_matrix_glyph(&bitmap_glyph_thumb_down,
                                     (LED_MATRIX_WIDTH - bitmap_glyph_thumb_down.width) / 2, (LED_MATRIX_HEIGHT - 8) / 2,
                                     LED_MATRIX_BRIGHTNESS_MAX);
                    target_frame = led_matrix_fb_switch(target_frame) + GLYPH_FRAMES;
                    led_matrix_wait_for_frame(target_frame);

                    static const char *msg_you_lost = "You lost";
                    led_matrix_text_scroll(&bitmap_font_matrix_light8, msg_you_lost, strlen(msg_you_lost), LED_MATRIX_BRIGHTNESS_MAX);
                    static const char *msg_your_score = "Your score:";
                    led_matrix_text_scroll(&bitmap_font_matrix_light8, msg_your_score, strlen(msg_your_score), LED_MATRIX_BRIGHTNESS_MAX);
                    char score[10];
                    size_t score_len = fmt_u32_dec(score, sequence_length);
                    led_matrix_text_scroll(&bitmap_font_matrix_light8, score, score_len, LED_MATRIX_BRIGHTNESS_MAX);
                    goto game_restart;
                }
                led_matrix_glyph(&glyph,
                                 (LED_MATRIX_WIDTH - glyph.width) / 2, (LED_MATRIX_HEIGHT - 8) / 2,
                                 LED_MATRIX_BRIGHTNESS_MAX);

                target_frame = led_matrix_fb_switch(target_frame);
                for (unsigned i = 0; i < GLYPH_FRAMES; i++) {
                    target_frame++;
                    led_matrix_wait_for_frame(target_frame);
                    button_matrix_scan(btns_pressed);
                    if (btns_pressed[0] == 0) {
                        if (i < GLYPH_MIN_FRAMES) {
                            target_frame += GLYPH_MIN_FRAMES - i;
                        }
                        break;
                    }
                }
                led_matrix_fb_clear();
                target_frame = led_matrix_fb_switch(target_frame);
            }
            target_frame += BLANK_FRAMES;
            led_matrix_fb_clear();
            led_matrix_glyph(&bitmap_glyph_thumb_up,
                             (LED_MATRIX_WIDTH - bitmap_glyph_thumb_up.width) / 2, (LED_MATRIX_HEIGHT - 8) / 2,
                             LED_MATRIX_BRIGHTNESS_MAX);
            target_frame = led_matrix_fb_switch(target_frame) + GLYPH_FRAMES;
            led_matrix_fb_clear();
            target_frame = led_matrix_fb_switch(target_frame) + BLANK_FRAMES;
        }
    }

    return 0;
}

#include <string.h>

#include "board.h"
#include "button_matrix.h"
#include "fmt.h"
#include "led_matrix.h"
#include "led_matrix_games.h"
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

static uint32_t flash_glyph_once(bitmap_glyph_t glyph, uint32_t target_frame)
{
    led_matrix_fb_clear();
    led_matrix_glyph(&glyph,
                     (LED_MATRIX_WIDTH - glyph.width) / 2, (LED_MATRIX_HEIGHT - 8) / 2,
                     LED_MATRIX_BRIGHTNESS_MAX);

    target_frame = led_matrix_fb_switch(target_frame) + GLYPH_FRAMES;
    led_matrix_fb_clear();
    return led_matrix_fb_switch(target_frame) + BLANK_FRAMES;
}

static uint32_t blink_glyph(bitmap_glyph_t glyph, uint32_t target_frame)
{
    for (unsigned loop = 0; loop < BLINK_LOOPS; loop++) {
        led_matrix_fb_clear();
        led_matrix_glyph(&glyph,
                         (LED_MATRIX_WIDTH - glyph.width) / 2, (LED_MATRIX_HEIGHT - 8) / 2,
                         LED_MATRIX_BRIGHTNESS_MAX);

        target_frame = led_matrix_fb_switch(target_frame) + BLINK_FRAMES;
        led_matrix_fb_clear();
        target_frame =  led_matrix_fb_switch(target_frame) + BLINK_FRAMES;
    }

    return target_frame;
}

static inline void show_failed(bitmap_glyph_t glyph, uint32_t sequence_length, uint32_t target_frame)
{
    target_frame = blink_glyph(glyph, target_frame);
    flash_glyph_once(bitmap_glyph_thumb_down, target_frame);

    static const char *msg_you_lost = "You lost";
    led_matrix_text_scroll(&bitmap_font_matrix_light8, msg_you_lost, strlen(msg_you_lost),
                           LED_MATRIX_BRIGHTNESS_MAX);
    static const char *msg_your_score = "Your score:";
    led_matrix_text_scroll(&bitmap_font_matrix_light8, msg_your_score, strlen(msg_your_score),
                           LED_MATRIX_BRIGHTNESS_MAX);
    char score[10];
    size_t score_len = fmt_u32_dec(score, sequence_length);
    led_matrix_text_scroll(&bitmap_font_matrix_light8, score, score_len, LED_MATRIX_BRIGHTNESS_MAX);
}

static uint32_t show_sequence(uint32_t seed, uint32_t sequence_length, uint32_t target_frame)
{
    for (uint32_t i = 0; i < sequence_length; i++) {
        bitmap_glyph_t glyph = simon_next_glyph(&seed);
        target_frame = flash_glyph_once(glyph, target_frame);
    }

    return target_frame;
}

static bool input_sequence_is_correct(uint32_t seed, uint32_t sequence_length, uint32_t *target_frame)
{
    uint8_t btns_pressed[1];

    for (uint32_t i = 0; i < sequence_length; i++) {
        do {
            button_matrix_scan(btns_pressed);
        } while (btns_pressed[0] == 0);

        led_matrix_fb_clear();
        bitmap_glyph_t glyph = glyph_by_key(btns_pressed[0]);
        uint8_t btn_correct = simon_next_btn(&seed);
        if (btns_pressed[0] != btn_correct) {
            show_failed(glyph, sequence_length, *target_frame);
            return false;
        }
        led_matrix_glyph(&glyph,
                         (LED_MATRIX_WIDTH - glyph.width) / 2, (LED_MATRIX_HEIGHT - 8) / 2,
                         LED_MATRIX_BRIGHTNESS_MAX);

        *target_frame = led_matrix_fb_switch(*target_frame);
        for (unsigned i = 0; i < GLYPH_FRAMES; i++) {
            *target_frame += 1;
            led_matrix_wait_for_frame(*target_frame);
            button_matrix_scan(btns_pressed);
            if (btns_pressed[0] == 0) {
                if (i < GLYPH_MIN_FRAMES) {
                    target_frame += GLYPH_MIN_FRAMES - i;
                }
                break;
            }
        }
        led_matrix_fb_clear();
        *target_frame = led_matrix_fb_switch(*target_frame);
    }
    *target_frame += BLANK_FRAMES;
    *target_frame = flash_glyph_once(bitmap_glyph_thumb_up, *target_frame);

    return true;
}

void led_matrix_games_ledmon_says(void)
{
    static const char *instructions = "Memorize the button sequence shown and enter it. Press any button to start";
    uint8_t btns_pressed[1];
    led_matrix_text_scroll_until_button(&bitmap_font_matrix_light8,
                                        instructions, strlen(instructions),
                                        board_btns_all, btns_pressed,
                                        sizeof(btns_pressed),
                                        LED_MATRIX_BRIGHTNESS_MAX);

    uint32_t target_frame;
    uint32_t seed;
    uint32_t sequence_length;

    target_frame = led_matrix_frame_number();
    seed = target_frame ^ 0x55555555;
    sequence_length = 0;

    while (1) {
        sequence_length++;

        target_frame = show_sequence(seed, sequence_length, target_frame);

        if (!input_sequence_is_correct(seed, sequence_length, &target_frame)) {
            return;
        }
    }
}

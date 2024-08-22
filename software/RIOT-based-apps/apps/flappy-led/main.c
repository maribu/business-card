#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include "board.h"
#include "button_matrix.h"
#include "compiler_hints.h"
#include "fmt.h"
#include "led_matrix.h"
#include "led_matrix_params.h"

#define BLINK_HALF_PERIOD   5
#define BLINKS_PER_STEP     1

struct obstacle {
    uint8_t gap_top;
    uint8_t gap_size;
    uint8_t distance;
};

static const uint8_t btns_any[] = {
    BUTTON_A | BUTTON_B | BUTTON_UP | BUTTON_DOWN | BUTTON_LEFT | BUTTON_RIGHT
};

static struct obstacle obstacles[8];
static uint8_t obstacles_numof = 0;
static uint8_t obstacles_pos = 0;
static uint8_t obstacles_distance = LED_MATRIX_WIDTH;
static uint8_t obstacle_offset = LED_MATRIX_WIDTH;
static uint8_t obstacle_min_gap = 4;
static int y_offset = 0;
static uint32_t seed;
static uint32_t score = 0;

static struct obstacle *obstacle(uint8_t pos)
{
    return &obstacles[(obstacles_pos + pos) & (ARRAY_SIZE(obstacles) - 1)];
}

static uint32_t xorshift32(uint32_t x)
{
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

static void draw_obstacles(void)
{
    int x = obstacle_offset;

    for (uint8_t i = 0; i < obstacles_numof; i++) {
        struct obstacle *o = obstacle(i);

        for (int y = 0; y < o->gap_top - y_offset; y++) {
            led_matrix_fb_set(x, y, LED_MATRIX_BRIGHTNESS_MAX);
        }

        for (int y = o->gap_top + o->gap_size - y_offset; y < (int)LED_MATRIX_HEIGHT; y++) {
            led_matrix_fb_set(x, y, LED_MATRIX_BRIGHTNESS_MAX);
        }

        x += (int)o->distance;
    }
}

static void spawn_abstacle(void)
{
    if (obstacles_numof >= ARRAY_SIZE(obstacles)) {
        return;
    }

    struct obstacle *o = obstacle(obstacles_numof);
    do {
        seed = xorshift32(seed);
        o->gap_size = obstacle_min_gap + (seed & 0x3);
        o->gap_top = (seed >> 2) & 0x7;
    } while (o->gap_top + o->gap_size + 1U >= LED_MATRIX_HEIGHT);

    o->distance = obstacles_distance;

    obstacles_numof++;
}

static bool step_and_is_crashing(void)
{
    if (obstacle_offset == 0) {
        assume(obstacles_numof > 0);
        obstacle_offset = obstacle(0)->distance - 1;
        obstacles_numof--;
        obstacles_pos++;
        if ((score & 0x3) == 0x0) {
            if (obstacles_distance > 3) {
                obstacles_distance--;
            }
        }
        score++;
        spawn_abstacle();
    }
    else {
        obstacle_offset--;
    }

    if (obstacle_offset == 1) {
        struct obstacle *o = obstacle(0);
        static const int y = LED_MATRIX_HEIGHT / 2;
        if ((y < o->gap_top - y_offset) || (y >= o->gap_top + o->gap_size - y_offset)) {
            return true;
        }
    }

    return false;
}

int main(void)
{
    int retval;

    retval = led_matrix_init();
    assert(retval == 0);

    retval = button_matrix_init();
    assert(retval == 0);
    (void)retval;

    static const char *msg = "Flappy LED: Press any button to gain altitude. Avoid crashing into obstacles.";
    uint8_t btns_pressed;

start_game:
    led_matrix_text_scroll_until_button(&bitmap_font_matrix_light8, msg, strlen(msg),
                                        btns_any, &btns_pressed,
                                        sizeof(btns_pressed),
                                        LED_MATRIX_BRIGHTNESS_MAX);

    uint32_t target_frame = led_matrix_frame_number();
    seed = target_frame ^ 0x55555555;

    for (unsigned i = 0; i < ARRAY_SIZE(obstacles); i++) {
        if ((i & 0x3) == 0x3) {
            obstacles_distance--;
        }
        spawn_abstacle();
    }

    while (1) {
        if (step_and_is_crashing()) {
            for (unsigned i = 0; i < 10; i++) {
                led_matrix_fb_clear();
                draw_obstacles();
                led_matrix_fb_set(1, LED_MATRIX_HEIGHT / 2, 0);
                target_frame = led_matrix_fb_switch(target_frame) + BLINK_HALF_PERIOD;

                led_matrix_fb_clear();
                draw_obstacles();
                led_matrix_fb_set(1, LED_MATRIX_HEIGHT / 2, LED_MATRIX_BRIGHTNESS_MAX);
                target_frame = led_matrix_fb_switch(target_frame) + BLINK_HALF_PERIOD;
            }

            led_matrix_wait_for_frame(target_frame);
            const char lost_msg[] = "You lost. Your score:";
            led_matrix_text_scroll(&bitmap_font_matrix_light8,
                                   lost_msg, sizeof(lost_msg) - 1, LED_MATRIX_BRIGHTNESS_MAX);
            char score_str[10];
            led_matrix_text_scroll(&bitmap_font_matrix_light8,
                                   score_str, fmt_u32_dec(score_str, score),
                                   LED_MATRIX_BRIGHTNESS_MAX);
            goto start_game;
        }

        uint8_t btns_old = 0;
        for (unsigned blink = 0; blink < BLINKS_PER_STEP; blink++) {
            uint8_t brightness = LED_MATRIX_BRIGHTNESS_MAX;
            for (unsigned j = 0; j < 2; j++) {
                for (unsigned i = 0; i < BLINK_HALF_PERIOD; i++) {
                    led_matrix_fb_clear();
                    draw_obstacles();
                    led_matrix_fb_set(1, LED_MATRIX_HEIGHT / 2, brightness);
                    target_frame = led_matrix_fb_switch(target_frame) + 1;
                    button_matrix_scan(&btns_pressed);
                    if (btns_pressed && (btns_pressed != btns_old)) {
                        y_offset -= 2;
                    }
                    btns_old = btns_pressed;
                }
                brightness = 1;
            }
        }

        y_offset++;

    }

    return 0;
}

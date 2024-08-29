#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "board.h"
#include "button_matrix.h"
#include "compiler_hints.h"
#include "fmt.h"
#include "led_matrix.h"
#include "led_matrix_params.h"
#include "led_matrix_games.h"

#define BLINK_HALF_PERIOD   5
#define BLINKS_PER_STEP     1

static struct flappy_led_game_data * const data = &led_matrix_games_data.flappy_led;

static struct flappy_led_obstacle *obstacle(uint8_t pos)
{
    return &data->obstacles[(data->obstacles_pos + pos) & (ARRAY_SIZE(data->obstacles) - 1)];
}

static void draw_obstacles(void)
{
    int x = data->first_obstacle_x;

    for (uint8_t i = 0; i < data->obstacles_numof; i++) {
        struct flappy_led_obstacle *o = obstacle(i);

        for (int y = 0; y < o->gap_top - data->y_offset; y++) {
            led_matrix_fb_set(x, y, LED_MATRIX_BRIGHTNESS_MAX);
        }

        for (int y = o->gap_top + o->gap_size - data->y_offset; y < (int)LED_MATRIX_HEIGHT; y++) {
            led_matrix_fb_set(x, y, LED_MATRIX_BRIGHTNESS_MAX);
        }

        x += (int)o->distance;
    }
}

static void spawn_abstacle(void)
{
    if (data->obstacles_numof >= ARRAY_SIZE(data->obstacles)) {
        return;
    }

    struct flappy_led_obstacle *o = obstacle(data->obstacles_numof);
    do {
        data->seed = xorshift32(data->seed);
        o->gap_size = data->obstacle_min_gap + (data->seed & 0x3);
        o->gap_top = (data->seed >> 2) & 0x7;
    } while (o->gap_top + o->gap_size + 1U >= LED_MATRIX_HEIGHT);

    o->distance = data->new_obstacle_distance;

    data->obstacles_numof++;
}

static bool step_and_is_crashing(void)
{
    if (data->first_obstacle_x == 0) {
        assume(data->obstacles_numof > 0);
        data->first_obstacle_x = obstacle(0)->distance - 1;
        data->obstacles_numof--;
        data->obstacles_pos++;
        if ((data->score & 0x3) == 0x0) {
            if (data->new_obstacle_distance > 3) {
                data->new_obstacle_distance--;
            }
        }
        data->score++;
        spawn_abstacle();
    }
    else {
        data->first_obstacle_x--;
    }

    if (data->first_obstacle_x == 1) {
        struct flappy_led_obstacle *o = obstacle(0);
        static const int y = LED_MATRIX_HEIGHT / 2;
        if ((y < o->gap_top - data->y_offset) || (y >= o->gap_top + o->gap_size - data->y_offset)) {
            return true;
        }
    }

    return false;
}

static void assert_obstancle_array_length_is_power_of_two(void)
{
    size_t o_max = ARRAY_SIZE(data->obstacles);
    /* in addition to checking for it being a power of two, we also
     * assert it to be at least four items long */
    assert((o_max >= 4) && ((o_max & (o_max - 1)) == 0));
}

void led_matrix_games_flappy_led(void)
{
    assert_obstancle_array_length_is_power_of_two();

    memset(data, 0, sizeof(*data));
    data->new_obstacle_distance = LED_MATRIX_WIDTH;
    data->first_obstacle_x = LED_MATRIX_WIDTH;
    data-> obstacle_min_gap = 4;

    static const char *instructions = "Press any button to gain altitude. Avoid crashing into obstacles.";
    uint8_t btns_pressed;

    led_matrix_text_scroll_until_button(&bitmap_font_matrix_light8,
                                        instructions, strlen(instructions),
                                        board_btns_all, &btns_pressed,
                                        sizeof(btns_pressed),
                                        LED_MATRIX_BRIGHTNESS_MAX);

    uint32_t target_frame = led_matrix_frame_number();
    data->seed = target_frame ^ 0x55555555;

    for (unsigned i = 0; i < ARRAY_SIZE(data->obstacles); i++) {
        if ((i & 0x3) == 0x3) {
            data->new_obstacle_distance--;
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
                                   score_str, fmt_u32_dec(score_str, data->score),
                                   LED_MATRIX_BRIGHTNESS_MAX);
            return;
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
                        data->y_offset -= 2;
                    }
                    btns_old = btns_pressed;
                }
                brightness = 1;
            }
        }

        data->y_offset++;

    }
}

#include <assert.h>
#include <string.h>

#include "board.h"
#include "button_matrix.h"
#include "led_matrix.h"
#include "led_matrix_games.h"
#include "led_matrix_params.h"

struct game {
    const char *name;
    void (*run)(void);
};

const struct game games[] = {
    {
        .name = "Flappy LED",
        .run = led_matrix_games_flappy_led,
    },
    {
        .name = "LEDmon says",
        .run = led_matrix_games_ledmon_says,
    },
};

int main(void)
{
    int retval;

    retval = led_matrix_init();
    assert(retval == 0);

    retval = button_matrix_init();
    assert(retval == 0);
    (void)retval;

    uint8_t game_idx = 0;
    const uint8_t btns = BUTTON_UP | BUTTON_DOWN | BUTTON_A;
    static const char msg_menu[] = "Menu";

    led_matrix_text_scroll(&bitmap_font_matrix_light8, msg_menu, sizeof(msg_menu) - 1,
                           LED_MATRIX_BRIGHTNESS_MAX);
    while (1) {
        const struct game *game = &games[game_idx];
        size_t len = strlen(game->name);
        uint8_t btns_pressed;
        led_matrix_text_scroll_until_button(&bitmap_font_matrix_light8, game->name, len,
                                            &btns, &btns_pressed, sizeof(btns_pressed),
                                            LED_MATRIX_BRIGHTNESS_MAX);

        led_matrix_fb_clear();

        if (btns_pressed & BUTTON_A) {
            /* wait for button to be released before starting the game */
            do {
                button_matrix_scan(&btns_pressed);
            } while (btns_pressed);

            while (1) {
                game->run();
            }
        }

        if (btns_pressed & BUTTON_UP) {
            game_idx--;
            if (game_idx >= ARRAY_SIZE(games)) {
                game_idx = ARRAY_SIZE(games) - 1;
            }
            led_matrix_glyph(&bitmap_glyph_arrow_up,
                             (LED_MATRIX_WIDTH - bitmap_glyph_arrow_up.width) / 2,
                             (LED_MATRIX_HEIGHT - 8) / 2,
                             LED_MATRIX_BRIGHTNESS_MAX);
        }
        else {
            game_idx++;
            if (game_idx >= ARRAY_SIZE(games)) {
                game_idx = 0;
            }
            led_matrix_glyph(&bitmap_glyph_arrow_down,
                             (LED_MATRIX_WIDTH - bitmap_glyph_arrow_up.width) / 2,
                             (LED_MATRIX_HEIGHT - 8) / 2,
                             LED_MATRIX_BRIGHTNESS_MAX);
        }

        led_matrix_fb_switch(led_matrix_frame_number());

        /* wait for the user to release the button */
        do {
            button_matrix_scan(&btns_pressed);
        } while (btns_pressed);
    }
}

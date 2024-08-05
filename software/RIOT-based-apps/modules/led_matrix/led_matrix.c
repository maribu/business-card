#include "atomic_utils.h"
#include "clk.h"
#include "irq.h"
#include "led_matrix.h"
#include "led_matrix_params.h"
#include "mineplex.h"
#include "periph/gpio_ll.h"
#include "periph/timer.h"

#include <string.h>

#define LED_MATRIX_BRIGHTNESS_BITS      2U
#define LED_MATRIX_BRIGHTNESS_LEVELS    (1U << LED_MATRIX_BRIGHTNESS_BITS)
#define LED_MATRIX_BRIGHTNESS_MAX       (LED_MATRIX_BRIGHTNESS_LEVELS - 1U)

#define LED_MATRIX_PRINT_FRAMES         5

static uword_t led_out_masks[LED_MATRIX_PIN_NUMOF];
static uword_t led_dir_masks[LED_MATRIX_PIN_NUMOF];
static uword_t led_out_mask_all;
static uword_t led_dir_mask_all;

static uint8_t fb1[(LED_MATRIX_LED_NUMOF * LED_MATRIX_BRIGHTNESS_BITS + 7) / 8];
static uint8_t fb2[(LED_MATRIX_LED_NUMOF * LED_MATRIX_BRIGHTNESS_BITS + 7) / 8];

static uint8_t *fb_showing = fb1;
static uint8_t *fb_preparing = fb2;

static uint32_t frames;

void led_matrix_fb_set(int x, int y, uint8_t brightness)
{
    if (((unsigned)x >= LED_MATRIX_WIDTH) || ((unsigned)y >= LED_MATRIX_HEIGHT)) {
        return;
    }

    size_t pos = ((size_t)x * LED_MATRIX_HEIGHT + (size_t)y) * LED_MATRIX_BRIGHTNESS_BITS;
    brightness &= LED_MATRIX_BRIGHTNESS_MAX;
    fb_preparing[pos >> 3] &= ~(LED_MATRIX_BRIGHTNESS_MAX << (pos & 0x7));
    fb_preparing[pos >> 3] |= brightness << (pos & 0x7);
}

void led_matrix_fb_clear(void)
{
    memset(fb_preparing, 0, sizeof(fb1));
}

void led_matrix_fb_switch(void)
{
    unsigned irq_state = irq_disable();
    uint8_t *tmp = fb_showing;
    fb_showing = fb_preparing;
    fb_preparing = tmp;
    irq_restore(irq_state);
}

static void led_timer_cb(void *arg, int chan)
{
    (void)arg;
    (void)chan;
    static unsigned x = 0;
    static unsigned y = 0;
    static unsigned b = 1;

    gpio_ll_switch_dir_input(LED_MATRIX_PORT, led_dir_mask_all);
    gpio_ll_clear(LED_MATRIX_PORT, led_out_mask_all);

    size_t pos = (x * LED_MATRIX_HEIGHT + y) * LED_MATRIX_BRIGHTNESS_BITS;

    if (((fb_showing[pos >> 3] >> (pos & 0x7)) & LED_MATRIX_BRIGHTNESS_MAX) >= b) {
        unsigned px = LED_MATRIX_WIDTH - 1 - x;
        unsigned py = (y >= px) ? y + 1 : y;
        gpio_ll_switch_dir_output(LED_MATRIX_PORT, led_dir_masks[px]);
        gpio_ll_switch_dir_output(LED_MATRIX_PORT, led_dir_masks[py]);
        gpio_ll_set(LED_MATRIX_PORT, led_out_masks[py]);
    }

    if (++y == LED_MATRIX_HEIGHT) {
        y = 0;
        if (++x == LED_MATRIX_WIDTH) {
            x = 0;
            if (++b == LED_MATRIX_BRIGHTNESS_LEVELS) {
                frames++;
                b = 1;
            }
        }
    }
}

int led_matrix_init(void)
{
    int retval;

    for (unsigned i = 0; i < LED_MATRIX_PIN_NUMOF; i++) {
        uword_t mask = 1U << led_matrix_pins[i];
        led_out_mask_all |= mask;
        led_dir_masks[i] = gpio_ll_prepare_switch_dir(mask);
        led_out_masks[i] = mask;
        gpio_conf_t conf = {
            .state = GPIO_INPUT,
            .pull = GPIO_FLOATING,
            .slew_rate = GPIO_SLEW_FASTEST,
        };
        retval = gpio_ll_init(LED_MATRIX_PORT, led_matrix_pins[i], conf);
        if (retval != 0) {
            return retval;
        }
    }

    led_dir_mask_all = gpio_ll_prepare_switch_dir(led_out_mask_all);

    uint64_t timer_freq = coreclk() >> 3;

    retval = timer_init(LED_MATRIX_TIMER, timer_freq, led_timer_cb, NULL);
    if (retval != 0) {
        return retval;
    }

    const uint32_t fps = 60;
    unsigned period = timer_freq / (fps * LED_MATRIX_LED_NUMOF * LED_MATRIX_BRIGHTNESS_MAX);
    return timer_set_periodic(LED_MATRIX_TIMER, 0, period,
                              TIM_FLAG_RESET_ON_MATCH | TIM_FLAG_RESET_ON_SET);
}

void led_matrix_wait_for_frame(uint32_t frame_number)
{
    while ((frame_number - atomic_load_u32(&frames)) <= UINT16_MAX) {
        /* spinning */
    }
}

uint32_t led_matrix_frame_number(void)
{
    return atomic_load_u32(&frames);
}

void led_matrix_print(const char *text, size_t text_len, uint8_t brightness)
{
    int xshift = LED_MATRIX_WIDTH - 1;

    uint32_t target = led_matrix_frame_number();

    while (1) {
        led_matrix_fb_clear();
        int yshift = 0;
        if (LED_MATRIX_HEIGHT >= 9) {
            for (unsigned x = 0; x < LED_MATRIX_WIDTH; x++) {
                led_matrix_fb_set(x, 0, 1);
                led_matrix_fb_set(x, 8, 1);
            }
            yshift = 2;
        }

        int xoffset = LED_MATRIX_WIDTH - xshift;
        for (int i = 0; i < (int)text_len; i++) {
            const uint8_t *font = mineplex_char(text[i]);
            int xmax = 0;
            for (int x = 0; x < 5; x++) {
                for (int y = 0; y < 5; y++) {
                    if (font[y] & (1U << x)) {
                        if (x > xmax) {
                            xmax = x;
                        }
                        led_matrix_fb_set(x + xoffset, y + yshift, brightness);
                    }
                }
            }

            xoffset += xmax + 2;
        }

        xshift++;
        if (xoffset <= -5) {
            return;
        }

        target += LED_MATRIX_PRINT_FRAMES;
        led_matrix_wait_for_frame(target);
        led_matrix_fb_switch();
    }
}

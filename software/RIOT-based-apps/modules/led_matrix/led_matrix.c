#include "atomic_utils.h"
#include "bitmap_fonts.h"
#include "clk.h"
#include "compiler_hints.h"
#include "irq.h"
#include "led_matrix.h"
#include "led_matrix_params.h"
#include "periph/gpio_ll.h"
#include "periph/timer.h"

#include <string.h>

#if MODULE_BUTTON_MATRIX
#include "button_matrix.h"
#include "button_matrix_params.h"
#endif /* MODULE_BUTTON_MATRIX */


#define LED_MATRIX_TEXT_SCROLL_FRAMES   4

static uword_t led_out_masks[LED_MATRIX_PIN_NUMOF];
static uword_t led_dir_masks[LED_MATRIX_PIN_NUMOF];
static uword_t led_out_mask_all;
static uword_t led_dir_mask_all;

static uint8_t fb1[(LED_MATRIX_LED_NUMOF * LED_MATRIX_BRIGHTNESS_BITS + 7) / 8];
static uint8_t fb2[(LED_MATRIX_LED_NUMOF * LED_MATRIX_BRIGHTNESS_BITS + 7) / 8];

static uint8_t *fb_active = fb1;
static uint8_t *fb_scratch = fb2;

static uint32_t frames;
static uint32_t frame_switch_target;
static uint8_t frame_switch_request;

void led_matrix_fb_set(int x, int y, uint8_t brightness)
{
    if (((unsigned)x >= LED_MATRIX_WIDTH) || ((unsigned)y >= LED_MATRIX_HEIGHT)) {
        return;
    }

    size_t pos = ((size_t)x * LED_MATRIX_HEIGHT + (size_t)y) * LED_MATRIX_BRIGHTNESS_BITS;
    brightness &= LED_MATRIX_BRIGHTNESS_MAX;
    fb_scratch[pos >> 3] &= ~(LED_MATRIX_BRIGHTNESS_MAX << (pos & 0x7));
    fb_scratch[pos >> 3] |= brightness << (pos & 0x7);
}

void led_matrix_fb_clear(void)
{
    memset(fb_scratch, 0, sizeof(fb1));
}

uint32_t led_matrix_fb_switch(uint32_t at_frame_number)
{
    unsigned irq_state = irq_disable();
    frame_switch_target = at_frame_number;
    frame_switch_request = 1;
    irq_restore(irq_state);

    while (atomic_load_u8(&frame_switch_request)) {
        /* busy wait */
    }

    /* The atomic_load_u32() will not provide thread-safety here,
     * as concurrent calls could already have changed the value.
     * But the doc says that only a single thread can do rendering.
     *
     * It more about telling the compiler to not optimized out the
     * load, as the ISR may have updated this if we are already
     * past this frame */
    return atomic_load_u32(&frame_switch_target);
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

    if (((fb_active[pos >> 3] >> (pos & 0x7)) & LED_MATRIX_BRIGHTNESS_MAX) >= b) {
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

                if (frame_switch_request && (frame_switch_target - frames > UINT16_MAX)) {
                    uint8_t *tmp = fb_active;
                    fb_active = fb_scratch;
                    fb_scratch = tmp;
                    frame_switch_target = frames;
                    frame_switch_request = 0;
                }
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

void led_matrix_glyph(const bitmap_glyph_t *glyph, int xoffset, int yoffset, uint8_t brightness)
{
    assert(glyph != NULL);

    if (xoffset + glyph->width < 0) {
        return;
    }

    if (xoffset >= (int)LED_MATRIX_WIDTH) {
        return;
    }

    for (int x = 0; x < glyph->width; x++) {
        for (int y = 0; y < 8; y++) {
            if (bitmap_glyph_at(glyph, x, y)) {
                led_matrix_fb_set(x + xoffset, y + yoffset, brightness);
            }
        }
    }

}

void led_matrix_text(const bitmap_font_t *font, const char *text, size_t len,
                     int xoffset, int yoffset, uint8_t brightness)
{
    assert((font != NULL) && (text != NULL));

    if (len == 0) {
        return;
    }

    bitmap_glyph_t left = bitmap_font_get(font, text[0]);

    led_matrix_glyph(&left, xoffset, yoffset, brightness);

    for (int i = 1; i < (int)len; i++) {
        bitmap_glyph_t right = bitmap_font_get(font, text[i]);
        xoffset += left.width;
        xoffset += bitmap_glyph_space_between(&left, &right);

        led_matrix_glyph(&right, xoffset, yoffset, brightness);

        left = right;
    }
}

void led_matrix_text_scroll(const bitmap_font_t *font, const char *text, size_t len,
                            uint8_t brightness)
{
    int xshift;
    int xshift_end = -(int)bitmap_font_render_width(font, text, len) - 1;
    int yshift = (LED_MATRIX_HEIGHT - 8 + 1) / 2;

    uint32_t frame_target = led_matrix_frame_number();

    led_matrix_fb_clear();

    for (xshift = LED_MATRIX_WIDTH - 1; xshift > xshift_end; xshift--) {
        led_matrix_text(font, text, len, xshift, yshift, brightness);
        frame_target = led_matrix_fb_switch(frame_target) + LED_MATRIX_TEXT_SCROLL_FRAMES;
        led_matrix_fb_clear();
    }
}

#if MODULE_BUTTON_MATRIX
void led_matrix_text_scroll_until_button(const bitmap_font_t *font,
                                         const char *text, size_t len,
                                         const uint8_t *btn_filter,
                                         uint8_t *btn_target,
                                         size_t btn_len,
                                         uint8_t brightness)
{
    assume((btn_filter != NULL) && (btn_target != NULL));
    assume(btn_len == (BUTTON_MATRIX_BUTTON_NUMOF + 7) / 8);
    int xshift;
    int xshift_end = -(int)bitmap_font_render_width(font, text, len) - 1;

    uint32_t frame_target = led_matrix_frame_number();

    led_matrix_fb_clear();

    while (1) {
        int yshift = (LED_MATRIX_HEIGHT - 8 + 1) / 2;
        for (xshift = LED_MATRIX_WIDTH - 1; xshift > xshift_end; xshift--) {
            led_matrix_text(font, text, len, xshift, yshift, brightness);
            frame_target = led_matrix_fb_switch(frame_target) + LED_MATRIX_TEXT_SCROLL_FRAMES;
            button_matrix_scan(btn_target);
            for (size_t i = 0; i < btn_len; i++) {
                if (btn_filter[i] & btn_target[i]) {
                    return;
                }
            }
            led_matrix_fb_clear();
        }
    }
}
#endif /* MODULE_BUTTON_MATRIX */

#include "nrf_log.h"

#include "led_pattern.h"


typedef enum {STATIC, RAINBOW, FADE, WORM} mode_type;

static uint8_t m_num_leds = 0;
static neopixel_strip_t * m_strip;

static mode_type m_segment_modes[4] = {STATIC, STATIC, STATIC, STATIC};
static uint16_t m_segment_base_hues[4] = {0, 0, 0, 0};
static uint16_t m_segment_current_hues[4] = {0, 0, 0, 0};
static uint8_t m_segment_hue_increments[4] = {0, 0, 0, 0};
static uint8_t m_segment_values[4] = {INITIAL_VALUE, INITIAL_VALUE, INITIAL_VALUE, INITIAL_VALUE};
static uint8_t m_segment_max_values[4] = {INITIAL_VALUE, INITIAL_VALUE, INITIAL_VALUE, INITIAL_VALUE};
static uint8_t m_segment_value_increments[4] = {5, 5, 5, 5};
static bool m_segment_fade_ups[4] = {false, false, false, false};


void led_pattern_init(neopixel_strip_t * strip, uint8_t num_leds)
{
    NRF_LOG_INFO("led_pattern_init\r\n");

    m_strip = strip;
    m_num_leds = num_leds;
    neopixel_clear(m_strip);
}


void led_pattern_static(uint8_t segment, uint8_t first_led, uint8_t last_led) {
    uint8_t r, g, b;

    fast_hsv2rgb_32bit(m_segment_current_hues[segment], SATURATION, m_segment_values[segment], &r, &g, &b);

    for (uint8_t led = first_led; led <= last_led; led++) {
        neopixel_set_color(m_strip, led, r, g, b);
    }
}


void led_pattern_rainbow(uint8_t segment, uint8_t first_led, uint8_t last_led) {
    uint8_t r, g, b;
    uint16_t hue;
    uint8_t tmp_led_num;

    for (uint8_t led = first_led; led <= last_led; led++) {
        tmp_led_num = led - first_led;

        hue = m_segment_current_hues[segment] + (tmp_led_num * RAINBOW_HUE_INTERVAL);
        if (hue >= HSV_HUE_MAX) {
          hue -= HSV_HUE_MAX;
        }

        fast_hsv2rgb_32bit(hue, SATURATION, m_segment_values[segment], &r, &g, &b);
        neopixel_set_color(m_strip, led, r, g, b);
    }
}


void led_pattern_fade(uint8_t segment, uint8_t first_led, uint8_t last_led) {
    uint8_t r, g, b;

    fast_hsv2rgb_32bit(m_segment_current_hues[segment], SATURATION, m_segment_values[segment], &r, &g, &b);

    for (uint8_t led = first_led; led <= last_led; led++) {
        neopixel_set_color(m_strip, led, r, g, b);
    }

    if (m_segment_fade_ups[segment]) {
        m_segment_values[segment] += m_segment_value_increments[segment];
        if (m_segment_values[segment] >= m_segment_max_values[segment]) {
            m_segment_fade_ups[segment] = false;
        }
    } else {
        if (m_segment_values[segment] < m_segment_value_increments[segment]) {
            m_segment_values[segment] = m_segment_value_increments[segment];
        }
        m_segment_values[segment] -= m_segment_value_increments[segment];
        if (m_segment_values[segment] == 0) {
            m_segment_fade_ups[segment] = true;
        }
    }
}


void led_pattern_worm(uint8_t segment, uint8_t first_led, uint8_t last_led) {
}


void led_pattern_step() {

  uint8_t first_led = 0;
  uint8_t last_led = 0;

  for (uint8_t segment=0; segment<4; segment++) {
    switch (segment) {
      case 0:
        first_led = TREE1_FIRST_LED;
        last_led = TREE1_LAST_LED;
        break;
      case 1:
        first_led = TREE2_FIRST_LED;
        last_led = TREE2_LAST_LED;
        break;
      case 2:
        first_led = TREE3_FIRST_LED;
        last_led = TREE3_LAST_LED;
        break;
      case 3:
        first_led = NOSE_LED;
        last_led = NOSE_LED;
        break;
    }
    switch (m_segment_modes[segment]) {
      case STATIC:
        led_pattern_static(segment, first_led, last_led);
        break;
      case RAINBOW:
        led_pattern_rainbow(segment, first_led, last_led);
        break;
      case FADE:
        led_pattern_fade(segment, first_led, last_led);
        break;
      case WORM:
        led_pattern_worm(segment, first_led, last_led);
        break;
    }
    m_segment_current_hues[segment] += m_segment_hue_increments[segment];
    if (m_segment_current_hues[segment] >= HSV_HUE_MAX) {
      m_segment_current_hues[segment] -= HSV_HUE_MAX;
    }
  }
}


void led_pattern_command_entry(uint8_t segment, uint8_t command, uint8_t data[]) {
    uint16_t hue;

    if (segment > 3) return;
    if (command < 1 || command > 6) return;

    switch (command) {
        // Mode
        case 1:
            if (data[0] > WORM) return;
            m_segment_modes[segment] = data[0];
            m_segment_current_hues[segment] = m_segment_base_hues[segment];
            break;
        // Base hue
        case 2:
            hue = (data[0] << 8) | data[1];
            if (hue > HSV_HUE_MAX) return;
            m_segment_base_hues[segment] = hue;
            m_segment_current_hues[segment] = hue;
            break;
        // Hue increment
        case 3:
            m_segment_hue_increments[segment] = data[0];
            if (data[0] == 0) {
                m_segment_current_hues[segment] = m_segment_base_hues[segment];
            }
            break;
        // Current value
        case 4:
            if (data[0] > MAX_VALUE) return;
            m_segment_values[segment] = data[0];
            break;
        // Max value
        case 5:
            if (data[0] > MAX_VALUE) return;
            m_segment_max_values[segment] = data[0];
            break;
        // Value increment
        case 6:
            m_segment_value_increments[segment] = data[0];
            break;
    }
}

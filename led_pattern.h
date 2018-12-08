#ifndef LED_PATTERN_H__
#define LED_PATTERN_H__

#include "neopixel.h"
#include "fast_hsv2rgb.h"

#define TREE1_FIRST_LED 0
#define TREE1_LAST_LED 6
#define NOSE_LED 7
#define TREE2_FIRST_LED 8
#define TREE2_LAST_LED 20
#define TREE3_FIRST_LED 21
#define TREE3_LAST_LED 27

#define SATURATION 255
#define MAX_VALUE 120
#define INITIAL_VALUE 100
#define RAINBOW_HUE_INTERVAL 100

// Tree 1 mode
// Tree 2 mode
// Tree 3 mode
// Nose mode

// Tree 1 base hue
// Tree 2 base hue
// Tree 3 base hue
// Nose base hue

// Tree 1 hue increment
// Tree 2 hue increment
// Tree 3 hue increment
// Nose hue increment

// Mode 0 - static
// Mode 1 - rainbow
// Mode 2 - fade
// Mode 3 - worm

// Bluetooth command
// Byte 1 - segment number trees == 1-3 / nose == 4
// Byte 2 - cmd (1 == mode, 2 == base hue, 3 == hue increment)
// If hue increment set to 0, reset current hue to base hue
// Byte 3 - value
// Byte 4 - value LSB for base hue


void led_pattern_init(neopixel_strip_t * strip, uint8_t num_leds);
void led_pattern_command_entry(uint8_t segment, uint8_t command, uint8_t data[]);
void led_pattern_step();

#endif

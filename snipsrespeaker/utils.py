from colour import Color
from math import ceil

def color_2_hex(color):
    return int(color.hex_l[1:], 16)

def is_in_bound(val, min_val, max_val):
    return (val >= min_val & val < max_val)

def get_brightness(strip, led_num):
    if (is_in_bound(led_num, 0, strip.num_led)):
        start_index = 4 * led_num
        tmp = float(strip.leds[start_index] & 0b00011111)
        tmp *= 100.0
        res = tmp / float(strip.global_brightness)
        return res
    return 0

def set_brightness(strip, led_num, value):
    if (value > 100):
        value = 100
    if (value < 0):
        value = 0
    if (is_in_bound(led_num, 0, strip.num_led)):
        brightness = ceil(value* strip.global_brightness / 100.0)
        brightness = int(brightness)
        ledstart = (brightness & 0b00011111) | strip.LED_START
        start_index = 4 * led_num
        strip.leds[start_index] = ledstart
        print("set %u to %u" % (led_num, brightness))

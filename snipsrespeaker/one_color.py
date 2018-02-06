""" Must append APA102_Pi path to PATH"""
import os
DIR = os.path.dirname(os.path.realpath(__file__)) + '/'
import sys
sys.path.append(DIR + "APA102_Pi/")

from colorcycletemplate import ColorCycleTemplate
from colour import Color
from math import ceil
import utils

class OneColor(ColorCycleTemplate):
    """Set all led to one Color"""
    def __init__(self, num_led, pause_value = 0, num_steps_per_cycle = 100,
                 num_cycles = -1, global_brightness = 255, color = Color("blue"),
                percent = 5, dim = 0):
        ColorCycleTemplate.__init__(self,num_led, pause_value,
                                    num_steps_per_cycle, num_cycles,
                                    global_brightness, order="rgb")

        self.color = Color(color)
        self.brightness = percent
        self.dim = dim

    def init(self, strip, num_led):
        for led in range(0, num_led):
            strip.set_pixel_rgb(led, utils.color_2_hex(self.color), self.brightness)

    def update(self, strip, num_led, num_steps_per_cycle, current_step,
               current_cycle):
        if (self.dim != 0):
            self.brightness -= self.dim
            if (self.brightness > 100):
                self.brightness  = 100
                self.dim *= -1
            if (self.brightness < 0):
                self.brightness  = 0
                self.dim *= -1
            for led in range(0, num_led):
                utils.set_brightness(strip, led, self.brightness)
            return 1
        return 0

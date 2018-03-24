""" Must append APA102_Pi path to PATH"""
import os
DIR = os.path.dirname(os.path.realpath(__file__)) + '/'
import sys
sys.path.append(DIR + "APA102_Pi/")

from colorcycletemplate import ColorCycleTemplate
from colour import Color
import utils

class ArrayColor(ColorCycleTemplate):
    """Set all according to an array of color"""
    def __init__(self, num_led, pause_value = 0, num_steps_per_cycle = 100,
                 num_cycles = -1, global_brightness = 255,
                 pixel = [(Color("blue"), 5)], rotate=0, dim=0, dim_array=[]):
        ColorCycleTemplate.__init__(self,num_led, pause_value,
                                    num_steps_per_cycle, num_cycles,
                                    global_brightness, order="rgb")

        self.pixel = list(map(lambda x:(Color(x[0]), x[1]), pixel))
        self.rotate = rotate
        self.dim = dim
        self.dim_array = dim_array
        self.brightness_array = [0,0,0,0,0,0,0,0,0,0,0,0]
        self.brightness = 0

    def init(self, strip, num_led):
        for led in range(0, min(len(self.pixel), num_led)):
            strip.set_pixel_rgb(led,
                                utils.color_2_hex(self.pixel[led][0]),
                                self.pixel[led][1])

    def update(self, strip, num_led, num_steps_per_cycle, current_step,
               current_cycle):
        res = 0
        if (self.rotate != 0):
            res = 1
            strip.rotate(self.rotate)
        if (self.dim != 0):
                res = 1
                self.brightness -= self.dim
                if (self.brightness > 100):
                    self.brigthness = 100
                    self.dim *= -1
                if (self.brightness < 0):
                    self.brightness = 0
                    self.dim *= -1
                for led in range(0, num_led):
                    utils.set_brightness(strip, led, self.brightness)
        if (len(self.dim_array)):
                res = 1
                for led in range(0, num_led - 1):
                    self.brightness_array[led] -= self.dim_array[led]
                    if (self.brightness_array[led] > 100):
                        self.brightness_array[led]  = 100
                        self.dim_array[led] *= -1
                    if (self.brightness_array[led] < 0):
                        self.brightness_array[led]  = 0
                        self.dim_array[led] *= -1
                    utils.set_brightness(strip, led, self.brightness_array[led])
        return res

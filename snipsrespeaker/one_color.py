
""" Must append APA102_Pi path to PATH"""
import sys
sys.path.append("APA102_Pi/")

from colorcycletemplate import ColorCycleTemplate
from colour import Color
import utils

class OneColor(ColorCycleTemplate):
    """Set all led to one Color"""
    def __init__(self, num_led, pause_value = 0, num_steps_per_cycle = 100,
                 num_cycles = -1, global_brightness = 255, color = Color("blue"),
                percent = 5):
        ColorCycleTemplate.__init__(self,num_led, pause_value,
                                    num_steps_per_cycle, num_cycles,
                                    global_brightness, order="rgb")

        self.color = Color(color)
        self.percent = percent
    def init(self, strip, num_led):
        for led in range(0, num_led):
            strip.set_pixel_rgb(led, utils.color_2_hex(self.color), self.percent)

    def update(self, strip, num_led, num_steps_per_cycle, current_step,
               current_cycle):
        return 0

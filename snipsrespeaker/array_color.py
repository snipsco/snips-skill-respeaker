
""" Must append APA102_Pi path to PATH"""
import sys
sys.path.append("APA102_Pi/")

from colorcycletemplate import ColorCycleTemplate
from colour import Color
import utils

class ArrayColor(ColorCycleTemplate):
    """Set all according to an array of color"""
    def __init__(self, num_led, pause_value = 0, num_steps_per_cycle = 100,
                 num_cycles = -1, global_brightness = 255,
                 pixel = [(Color("blue"), 5)]):
        ColorCycleTemplate.__init__(self,num_led, pause_value,
                                    num_steps_per_cycle, num_cycles,
                                    global_brightness, order="rgb")

        self.pixel = list(map(lambda x:(Color(x[0]), x[1]), pixel))

    def init(self, strip, num_led):
        for led in range(0, min(len(self.pixel), num_led)):
            strip.set_pixel_rgb(led,
                                utils.color_2_hex(self.pixel[led][0]),
                                self.pixel[led][1])

    def update(self, strip, num_led, num_steps_per_cycle, current_step,
               current_cycle):
        return 0

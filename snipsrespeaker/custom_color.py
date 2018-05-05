from array_color import ArrayColor

class CustomColor:

    def __init__(self, num_led, pause_value = 0, num_steps_per_cycle = 1,
                 num_cycles = 1, global_brightness = 255,
                 pixel=[[("blue", 5)]]):
        self.array_color = list(
            map(lambda x: ArrayColor(num_led = num_led,
                                    pause_value = pause_value,
                                    num_steps_per_cycle = 1,
                                    num_cycles = 1,
                                    global_brightness = global_brightness,
                                    pixel= x), pixel))

    def init(self, strip, num_led):
        for color in self.array_color:
            color.init(self, strip, num_led)

    def shutdown(self, strip, num_led):
        for color in self.array_color:
            color.shutdown(strip, num_led)

    def update(self, strip, num_led, num_steps_per_cycle, current_step,
               current_cycle):
        print "toto"
        for color in self.array_color:
            color.update(strip, num_led, num_step_per_cycle, current_step,
                        current_cycle)

    def cleanup(self, strip):
        self.array_color[0].cleanup(strip)

    def start(self):
        for color in self.array_color:
            color.start()

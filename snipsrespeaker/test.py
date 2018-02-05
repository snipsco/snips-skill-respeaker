from one_color import OneColor
from array_color import ArrayColor

NUM_LED = 3

MY_CYCLE = OneColor(num_led=NUM_LED, pause_value=3,
                    num_steps_per_cycle=10, num_cycles=5, color="green", dim =5)
MY_CYCLE.start()


MY_CYCLE = ArrayColor(num_led=NUM_LED, pause_value=3,
                    num_steps_per_cycle=10, num_cycles=5, pixel=[("blue", 5),
                                                                ("yellow", 6),
                                                                ("green", 30)], dim = 5)
MY_CYCLE = ArrayColor(num_led=NUM_LED, pause_value=3,
                    num_steps_per_cycle=10, num_cycles=5, pixel=[("blue", 100),
                                                                ("green", 100),
                                                                ("blue", 100)],
                    dim_array = [10, 5, 1])
MY_CYCLE.start()


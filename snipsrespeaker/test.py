from one_color import OneColor

NUM_LED = 3

MY_CYCLE = OneColor(num_led=NUM_LED, pause_value=3,
                    num_steps_per_cycle=1, num_cycles=1, color="green")
MY_CYCLE.start()

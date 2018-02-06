from array_color import ArrayColor
import json
from math import ceil
from one_color import OneColor
from pprint import pprint
import threading
import time
import Queue
import os

DIR = os.path.dirname(os.path.realpath(__file__)) + '/'

class SnipsRespeaker:
    queue = Queue.Queue()
    state_working = None
    state_working = None

    @staticmethod
    def get_num_step(dim):
        return ceil(100 / dim) * 2

    @staticmethod
    def parse_one_color(data, num_led):
        color = data["color"]
        dim = int(data["dim"])
        idle_time = float(data["idle_time"])
        res = OneColor(num_led = num_led,
                       pause_value = idle_time,
                       num_steps_per_cycle = 1,
                       num_cycles = SnipsRespeaker.get_num_step(dim),
                       color = color,
                       dim = dim,
                       percent = 0)
        return res

    @staticmethod
    def parse_array_color(data, num_led):
        color = data["color"]
        dim = data["dim"]
        idle_time = float(data["idle_time"])
        rotate = int(data["rotate"])
        pixel = list(map(lambda x:(x, 0),color))
        if (rotate != 0):
            num_cycle = num_led / rotate + 1
        if (type(dim) is list):
            if (num_cycle == 0):
                num_cycle = SnipsRespeaker.get_num_step(min(dim)) 
            dim_array = dim
            dim_value = 0
        else:
            if (num_step == 0):
                num_cycle = get_num_step(dim) 
            dim_array = []
            dim_value = dim
        res = ArrayColor(num_led = num_led,
                         pause_value = idle_time,
                         num_steps_per_cycle = 1,
                         num_cycles = num_cycle,
                         pixel = pixel,
                         dim_array = dim_array,
                         dim = dim_value,
                         rotate = rotate)
        return res
    @staticmethod
    def parse_state(data, led_num):
        if (data["type"] == "one_color"):
            return SnipsRespeaker.parse_one_color(data, led_num)
        if (data["type"] == "array"):
            return SnipsRespeaker.parse_array_color(data, led_num)

    @staticmethod
    def worker():
        item = ""
        while True:
            if (not SnipsRespeaker.queue.empty()):
                item = SnipsRespeaker.queue.get_nowait()
                SnipsRespeaker.queue.task_done()
            if (item == "working"):
                SnipsRespeaker.state_working.start()
            if (item == "waiting"):
                SnipsRespeaker.state_waiting.start()

    def __init__(self, num_led=3, config_file="config.json", locale=None):
        with open(DIR + config_file) as f:
            data = json.load(f)
        SnipsRespeaker.state_waiting = SnipsRespeaker.parse_state(data["waiting"], num_led)
        SnipsRespeaker.state_working = SnipsRespeaker.parse_state(data["working"], num_led)
        SnipsRespeaker.queue.put("waiting")
        t = threading.Thread(target=SnipsRespeaker.worker, args=())
        t.start()

    def hotword_detected(self):
        print("hotword detected")
        SnipsRespeaker.queue.put("working")

    def stop_working(self):
        print("hotword detected")
        SnipsRespeaker.queue.put("waiting")

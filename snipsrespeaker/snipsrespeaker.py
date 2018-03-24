from array_color import ArrayColor
from custom_color import CustomColor
import json
from math import ceil
from one_color import OneColor
import os
from pprint import pprint
import threading
import time
import Queue
DIR = os.path.dirname(os.path.realpath(__file__)) + '/'
import RPi.GPIO as GPIO
import paho.mqtt.publish as publish
import socket
import signal
import sys
import subprocess

mqtt_master = 'localhost'

""" LED """
sys.path.append(DIR + "APA102_Pi/")

from colorcycletemplate import ColorCycleTemplate
from colour import Color
import utils

class SnipsRespeaker:
    queue = Queue.Queue()
    state_working = None
    state_stopping = None
    state_waiting = None
    state_running = None

    @staticmethod
    def get_num_step(dim):
        return ceil(100 / dim) * 2

    @staticmethod
    def parse_custom_color(data, num_led):
        color = data["color"]
        idle_time = float(data["idle_time"])
        pixel = [map(lambda x:(x, 0.2),c) for c in color]
        res = CustomColor(num_led = num_led,
                         pause_value = idle_time,
                         num_steps_per_cycle = 1,
                         num_cycles = -1,
                         pixel = pixel)
        return res

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
        if (data["type"] == "custom"):
            return SnipsRespeaker.parse_custom_color(data, led_num)

    @staticmethod
    def worker():
        item = ""
        while True:
            if (not SnipsRespeaker.queue.empty()):
                item = SnipsRespeaker.queue.get_nowait()
                SnipsRespeaker.queue.task_done()
            if (item == "working"):
                if (SnipsRespeaker.state_working is not None):
                    SnipsRespeaker.state_working.start()
            if (item == "waiting"):
                if (SnipsRespeaker.state_waiting is not None):
                    SnipsRespeaker.state_waiting.start()
            if (item == "stopping"):
                if (SnipsRespeaker.state_stopping is not None):
                    SnipsRespeaker.state_stopping.start()
            if (item == "running"):
                if (SnipsRespeaker.state_running is not None):
                    SnipsRespeaker.state_running.start()

    def __init__(self, config_file=DIR + "config.json", locale=None):
        num_led = 0

        p = subprocess.Popen(['arecord', '-l'], stdout=subprocess.PIPE)
        out, err = p.communicate()
        if (out.find("seeed-4mic-voicecard") != -1):
            num_led = 12
        if (out.find("seeed-2mic-voicecard") != -1):
            num_led = 3
        if (num_led == 0):
            print("no Respeaker Hat installed")
            return
        with open(config_file) as f:
            data = json.load(f)
        if ("waiting" in data):
            SnipsRespeaker.state_waiting = SnipsRespeaker.parse_state(data["waiting"], num_led)
        if ("working" in data):
            SnipsRespeaker.state_working = SnipsRespeaker.parse_state(data["working"], num_led)
        if ("stopping" in data):
            SnipsRespeaker.state_stopping = SnipsRespeaker.parse_state(data["stopping"], num_led)
        if ("running" in data):
            SnipsRespeaker.state_running = SnipsRespeaker.parse_state(data["running"], num_led)
        SnipsRespeaker.queue.put("working")
        SnipsRespeaker.queue.put("waiting")
        t = threading.Thread(target=SnipsRespeaker.worker, args=())
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(17, GPIO.IN, pull_up_down=GPIO.PUD_UP)
	GPIO.add_event_detect(17, GPIO.FALLING, callback=hotword_toggle, bouncetime=300)
        t.start()

    def hotword_detected(self):
        SnipsRespeaker.queue.put("working")

    def stop_working(self):
        SnipsRespeaker.queue.put("waiting")

def hotword_turn_off():
        SnipsRespeaker.queue.put("stopping")
        SnipsRespeaker.queue.put("nothing")
	p = subprocess.Popen(["systemctl", "stop", "snips-audio-server.service"])
	p.wait()

def hotword_turn_on():
        SnipsRespeaker.queue.put("running")
        SnipsRespeaker.queue.put("waiting")
	p = subprocess.Popen(["systemctl", "start", "snips-audio-server.service"])
	p.wait()

def static_vars(**kwargs):
	def decorate(func):
		for k in kwargs:
			setattr(func, k, kwargs[k])
		return func
	return decorate

@static_vars(toggled=True)
def hotword_toggle(toggled):
	hotword_toggle.toggled = not hotword_toggle.toggled
	if hotword_toggle.toggled:
		print "starting the hotword detection"
		hotword_turn_on()
	else:
		print "stopping the hotword detection"
		hotword_turn_off()

if __name__ == "__main__":
	SnipsRespeaker()

#!/usr/bin/env python2
# -*-: coding utf-8 -*-

from hermes_python.hermes import Hermes
from snipsrespeaker.snipsrespeaker import SnipsRespeaker
from thread_handler import ThreadHandler
from config_parser import SnipsConfigParser

MQTT_IP_ADDR = "localhost"
MQTT_PORT = 1883
MQTT_ADDR = "{}:{}".format(MQTT_IP_ADDR, str(MQTT_PORT))

class Skill:

    def __init__(self):
        self.thread_handler = ThreadHandler()

        config = SnipsConfigParser.read_configuration_file('config.ini')
        if config.get('global') is not None:
            customConfig = config['global']['configuration']
            self.respeaker = SnipsRespeaker(customConfig)
        else:
            self.respeaker = SnipsRespeaker()

        self.thread_handler.run(target=self.start_blocking)
        self.thread_handler.start_run_loop()

    def start_blocking(self, run_event):
        while run_event.is_set():
            with Hermes(MQTT_ADDR) as h:
                h.subscribe_session_started(self.action_session_started) \
                    .subscribe_session_ended(self.action_session_ended) \
                    .start()

    def action_session_started(self, hermes, session):
        print('Session STARTED')
        self.thread_handler.run(target=self.dialogue_session_started)

    def action_session_ended(self, hermes, session):
        print('Session ENDED')
        self.thread_handler.run(target=self.dialogue_session_ended)

    def dialogue_session_started(self, run_event):
        self.respeaker.hotword_detected()

    def dialogue_session_ended(self, run_event):
        self.respeaker.stop_working()


if __name__ == "__main__":
    Skill()

#!/usr/bin/env python2
# -*-: coding utf-8 -*-

from hermes_python.hermes import Hermes
from snipsrespeaker.snipsrespeaker import SnipsRespeaker
import ConfigParser
import io

CONFIGURATION_ENCODING_FORMAT = "utf-8"
CONFIG_INI = "conf.ini"

MQTT_IP_ADDR = "localhost"
MQTT_PORT = 1883
MQTT_ADDR = "{}:{}".format(MQTT_IP_ADDR, str(MQTT_PORT))


class SnipsConfigParser(ConfigParser.SafeConfigParser):
    def to_dict(self):
        return {section: {option_name : option for option_name, option in self.items(section)} for section in self.sections()}


def read_configuration_file(configuration_file):
    try:
        with io.open(configuration_file, encoding=CONFIGURATION_ENCODING_FORMAT) as f:
            conf_parser = SnipsConfigParser()
            conf_parser.readfp(f)
            return conf_parser.to_dict()
    except (IOError, ConfigParser.Error) as e:
        print(e)
        return dict()


def action_session_started(hermes, session):
    SnipsRespeaker.hotword_toggle()

def action_session_ended(hermes, session):
    SnipsRespeaker.hotword_toggle()

with Hermes(MQTT_ADDR) as h:
    SnipsRespeaker()

    h.subscribe_session_started() \
     .subscribe_session_ended() \
     .start()

        # conf = read_configuration_file(CONFIG_INI)
        #
        # if 'hostname' in conf.values() is False:
        #     print('No hostname specified in the conf.ini')
        # if 'username' in conf.values() is False:
        #     print('No username specified in the conf.ini')
        #
        # hostname = conf['secret']['hostname']
        # username = conf['secret']['username']
        # print(username)
        #
        # global snipshue
        #try:
        #    snipshue = SnipsHue(hostname, None)
        #except Exception as e:
        #    print('error: ' + e)
        # h.subscribe_intent('lightsTurnOff', lights_turn_off) \
        #     .subscribe_intent('lightsTurnOnSet', lights_turn_on_set) \
        #     .subscribe_intent('lightsTurnDown', lights_turn_down) \
        #     .subscribe_intent('lightsTurnUp', lights_turn_up) \
        #     .start()

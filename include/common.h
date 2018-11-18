#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>

#define NUM_TOPIC 		10
#define HOT_ON  			"hermes/hotword/toggleOn"
#define HOT_OFF 			"hermes/hotword/toggleOff"
#define STA_LIS 			"hermes/asr/startListening"
#define END_LIS 			"hermes/asr/stopListening"
#define STA_SAY 			"hermes/tts/say"
#define END_SAY 			"hermes/tts/sayFinished"
#define SUD_ON  			"hermes/feedback/sound/toggleOn"
#define SUD_OFF 			"hermes/feedback/sound/toggleOff"
#define LED_ON  			"hermes/feedback/led/toggleOn"
#define LED_OFF 			"hermes/feedback/led/toggleOff"

#define STATE_NUM 			  6
#define ON_IDLE_STR 		  "on_idle"
#define ON_LISTEN_STR 	  "on_listen"
#define ON_SPEAK_STR 		  "on_speak"
#define TO_MUTE_STR 		  "to_mute"
#define TO_UNMUTE_STR 	  "to_unmute"
#define ON_DISABLED_STR		"on_disabled"

typedef enum {
  ON_IDLE = 0,
  ON_LISTEN,
  ON_SPEAK,
  TO_MUTE,
  TO_UNMUTE,
  ON_DISABLED
}STATE;

#define CONFIG_NUM 			    16
#define C_MODEL_STR 	 	    "model"
#define C_SPI_DEV_STR 		  "spi_dev"
#define C_LED_NUM_STR 		  "led_num"
#define C_LED_BRI_STR 		  "led_bri"
#define C_MQTT_HOST_STR 	  "mqtt_host"
#define C_MQTT_PORT_STR 	  "mqtt_port"
#define C_ON_IDLE_STR 		  "on_idle"
#define C_ON_LISTEN_STR 	  "on_listen"
#define C_ON_SPEAK_STR 		  "on_speak"
#define C_TO_MUTE_STR 		  "to_mute"
#define C_TO_UNMUTE_STR 	  "to_unmute"
#define C_NIGHTMODE_STR 	  "nightmode"
#define C_GO_SLEEP_STR 		  "go_sleep"
#define C_GO_WEAK_STR 		  "go_weak"
#define C_ON_DISABLED_STR 	"on_disabled"
#define C_SITE_ID_STR 		  "site_id"

enum CONFIG_ENTITY{
  C_MODEL = 0,
  C_SPI_DEV,
  C_LED_NUM,
  C_LED_BRI,
  C_MQTT_HOST,
  C_MQTT_PORT,
  C_ON_IDLE,
  C_ON_LISTEN,
  C_ON_SPEAK,
  C_TO_MUTE,
  C_TO_UNMUTE,
  C_NIGHTMODE,
  C_GO_SLEEP,
  C_GO_WEAK,
  C_ON_DISABLED,
  C_SITE_ID
};

#define TRUE_S		"true"
#define FALSE_S		"false"

#define CONFIG_FILE "config.ini"

typedef struct{
  char key[20];
  char value[50];
}snipsSkillConfig;

#define CLIENT_ID_LEN 10

typedef struct{
  uint8_t numLEDs;
  int     fd_spi;
  uint8_t *pixels;
  uint8_t brightness;
}APA102;

#endif
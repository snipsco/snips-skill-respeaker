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

#define HOT_ON  "hermes/hotword/toggleOn"
#define HOT_OFF "hermes/hotword/toggleOff"
#define STA_LIS "hermes/asr/startListening"
#define END_LIS "hermes/asr/stopListening"
#define STA_SAY "hermes/tts/say"
#define END_SAY "hermes/tts/sayFinished"
#define SUD_ON  "hermes/feedback/sound/toggleOn"
#define SUD_OFF "hermes/feedback/sound/toggleOff"
#define LED_ON  "hermes/feedback/led/toggleOn"
#define LED_OFF "hermes/feedback/led/toggleOff"

#define NUM_TOPIC 10

#define ON_IDLE_STR 		"on_idle"
#define ON_LISTEN_STR 		"on_listen"
#define ON_SPEAK_STR 		"on_speak"
#define TO_MUTE_STR 		"to_mute"
#define TO_UNMUTE_STR 		"to_unmute"
#define ON_DISABLED_STR		"on_disabled"

#define STATE_NUM 6

#define CONFIG_NUM 18

#define CLIENT_ID_LEN 10

typedef struct{
	uint8_t numLEDs;
	int     fd_spi;
	uint8_t *pixels;
	uint8_t brightness;
}APA102;

typedef struct{
	char key[20];
	char value[50];
}snipsSkillConfig;

typedef enum {
	ON_IDLE = 0,
	ON_LISTEN,
	ON_SPEAK,
	TO_MUTE,
	TO_UNMUTE,
	ON_DISABLED
}STATE;
#endif
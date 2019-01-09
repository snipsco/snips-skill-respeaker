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
#include <signal.h>

#define APP "snips-led-animation"
#define VERSION "1.3.1"
#define LAST_UPDATE "JAN-09-2019"
#define AUTHOR "KE FANG (SNIPS)"

#define NUM_TOPIC       10
#define HOT_ON          "hermes/hotword/toggleOn"
#define HOT_OFF         "hermes/hotword/toggleOff"
#define STA_LIS         "hermes/asr/startListening"
#define END_LIS         "hermes/asr/stopListening"
#define STA_SAY         "hermes/tts/say"
#define END_SAY         "hermes/tts/sayFinished"
#define SUD_ON          "hermes/feedback/sound/toggleOn"
#define SUD_OFF         "hermes/feedback/sound/toggleOff"
#define LED_ON          "hermes/feedback/led/toggleOn"
#define LED_OFF         "hermes/feedback/led/toggleOff"

#define STATE_NUM       6
#define ON_IDLE_STR     "on_idle"
#define ON_LISTEN_STR   "on_listen"
#define ON_SPEAK_STR    "on_speak"
#define TO_MUTE_STR     "to_mute"
#define TO_UNMUTE_STR   "to_unmute"
#define ON_DISABLED_STR "on_disabled"

typedef enum {
    ON_IDLE = 0,
    ON_LISTEN,
    ON_SPEAK,
    TO_MUTE,
    TO_UNMUTE,
    ON_DISABLED
}STATE;

#define CONFIG_NUM              23
#define C_MODEL_STR             "model"
#define C_SPI_DEV_STR           "spi_dev"
#define C_LED_NUM_STR           "led_num"
#define C_LED_BRI_STR           "led_bri"
#define C_MQTT_HOST_STR         "mqtt_host"
#define C_MQTT_PORT_STR         "mqtt_port"
#define C_MQTT_USER_STR         "mqtt_username"
#define C_MQTT_PASS_STR         "mqtt_password"
#define C_ON_IDLE_STR           "on_idle"
#define C_ON_LISTEN_STR         "on_listen"
#define C_ON_SPEAK_STR          "on_speak"
#define C_TO_MUTE_STR           "to_mute"
#define C_TO_UNMUTE_STR         "to_unmute"
#define C_IDLE_COLOUR_STR       "idle_colour"
#define C_LISTEN_COLOUR_STR     "listen_colour"
#define C_SPEAK_COLOUR_STR      "speak_colour"
#define C_MUTE_COLOUR_STR       "mute_colour"
#define C_UNMUTE_COLOUR_STR     "unmute_colour"
#define C_NIGHTMODE_STR         "nightmode"
#define C_GO_SLEEP_STR          "go_sleep"
#define C_GO_WEAK_STR           "go_weak"
#define C_ON_DISABLED_STR       "on_disabled"
#define C_SITE_ID_STR           "site_id"
#define C_FEEDBACK_SOUND_STR    "feedback_sound"

enum CONFIG_ENTITY{
    C_MODEL = 0,
    C_SPI_DEV,
    C_LED_NUM,
    C_LED_BRI,
    C_MQTT_HOST,
    C_MQTT_PORT,
    C_MQTT_USER,
    C_MQTT_PASS,
    C_ON_IDLE,
    C_ON_LISTEN,
    C_ON_SPEAK,
    C_TO_MUTE,
    C_TO_UNMUTE,
    C_IDLE_COLOUR,
    C_LISTEN_COLOUR,
    C_SPEAK_COLOUR,
    C_MUTE_COLOUR,
    C_UNMUTE_COLOUR,
    C_NIGHTMODE,
    C_GO_SLEEP,
    C_GO_WEAK,
    C_ON_DISABLED,
    C_SITE_ID
};

#define TRUE_S    "true"
#define FALSE_S   "false"

#define CONFIG_FILE    "config.ini"

#define CLIENT_ID_LEN 10

#define RED_C    0xFF0000
#define GREEN_C  0x00FF00
#define BLUE_C   0x0000FF
#define YELLOW_C 0xFFFF00
#define PURPLE_C 0xFF00FF
#define TEAL_C   0x00FFFF
#define ORANGE_C 0xFF8000

typedef struct{
    uint32_t idle;
    uint32_t listen;
    uint32_t speak;
    uint32_t mute;
    uint32_t unmute;
}COLOURS;

typedef struct{
    int number;
    int spi_bus;
    int spi_dev;
}HW_LED_SPEC;

typedef struct{
    int pin;
    int val;
}HW_GPIO_SPEC;

typedef struct{
    /* Hardware */
    char hardware_model[50];
    HW_LED_SPEC LEDs;
    HW_GPIO_SPEC power;
    HW_GPIO_SPEC button;

    /* Brightness */
    uint8_t max_brightness;
    /* MQTT connection */
    char mqtt_host[50];
    char mqtt_port[50];
    char mqtt_user[50];
    char mqtt_pass[50];

    /* SiteId */
    char snips_site_id[50];

    /* Client ID */
    char *client_id;

    /* Animation thread */
    pthread_t curr_thread;
    STATE     curr_state;

    /* Colour */
    COLOURS animation_color;

    /* Sleep mode */
    uint8_t sleep_hour;
    uint8_t sleep_minute;
    uint8_t weak_hour;
    uint8_t weak_minute;

    /* Flags */
    volatile sig_atomic_t   if_terminate;
    uint8_t                 if_update;
    uint8_t                 if_sleepmode;

    /* Animation Enable */
    uint8_t animation_enable[STATE_NUM];

    /* Feedback sound */
    uint8_t if_mute;
}SNIPS_RUN_PARA;

#define CLIENT_ID_LEN 10

#endif

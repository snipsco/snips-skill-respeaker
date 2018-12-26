#include "apa102.h"
#include "get_config.h"
#include "state_handler.h"
#include "mqtt_client.h"

#include <mqtt.h>
#include <common.h>
#include <pthread.h>
#include <signal.h>

#define CLIENT_ID_LEN 10

static void interrupt_handler(int sig);

void check_nightmode(void);
char *generate_client_id(void);
void close_all(int status);
void get_action_colours();

volatile sig_atomic_t   flag_terminate = 0;
short                   flag_update = 1;
short                   flag_sleepmode = 0;

APA102      leds = {0, -1, NULL, 127};
STATE       curr_state = ON_IDLE;
COLOURS     action_colours = {GREEN_C, BLUE_C, PURPLE_C, YELLOW_C, GREEN_C};

pthread_t   curr_thread;
uint8_t     sleep_hour;
uint8_t     sleep_minute;
uint8_t     weak_hour;
uint8_t     weak_minute;

const char	*addr;
const char	*port;
const char  *username;
const char  *password;

const char  *site_id;

snipsSkillConfig configList[CONFIG_NUM]=
{
    {{C_MODEL_STR}, {0}},
    {{C_SPI_DEV_STR}, {0}},
    {{C_LED_NUM_STR}, {0}},
    {{C_LED_BRI_STR}, {0}},
    {{C_MQTT_HOST_STR}, {0}},
    {{C_MQTT_PORT_STR}, {0}},
    {{C_MQTT_USER_STR}, {0}},
    {{C_MQTT_PASS_STR}, {0}},
    {{C_ON_IDLE_STR}, {0}},
    {{C_ON_LISTEN_STR}, {0}},
    {{C_ON_SPEAK_STR}, {0}},
    {{C_TO_MUTE_STR}, {0}},
    {{C_TO_UNMUTE_STR}, {0}},
    {{C_IDLE_COLOUR_STR}, {0}},
    {{C_LISTEN_COLOUR_STR}, {0}},
    {{C_SPEAK_COLOUR_STR}, {0}},
    {{C_MUTE_COLOUR_STR}, {0}},
    {{C_UNMUTE_COLOUR_STR}, {0}},
    {{C_NIGHTMODE_STR}, {0}},
    {{C_GO_SLEEP_STR}, {0}},
    {{C_GO_WEAK_STR}, {0}},
    {{C_ON_DISABLED_STR}, {"1"}},
    {{C_SITE_ID_STR}, {0}}
};

int main(int argc, char const *argv[])
{
    char *client_id;
    // generate a random id as client id
    client_id = generate_client_id();
    signal(SIGINT, interrupt_handler);
    // get config.ini
    read_config_file(configList, CONFIG_NUM);

    // get input parameters
    leds.numLEDs = (argc > 1)? atoi(argv[1]) : atoi(configList[C_LED_NUM].value);
    addr = (argc > 2)? argv[2] : configList[C_MQTT_HOST].value; // mqtt_host
    port = (argc > 3)? argv[3] : configList[C_MQTT_PORT].value; // mqtt_port
    username = (argc > 4)? argv[4] : configList[C_MQTT_USER].value; // mqtt_username
    password = (argc > 5)? argv[5] : configList[C_MQTT_PASS].value; // mqtt_password
    site_id = (argc > 6)? argv[6] : configList[C_SITE_ID].value; // siteId
    // get brightness
    leds.brightness = (strlen(configList[C_LED_BRI].value) != 0) ? atoi(configList[C_LED_BRI].value) : 127;
    get_action_colours();

    // if sleep mode is enabled
    if (if_config_true("nightmode", configList, NULL) == 1){
        flag_sleepmode = 1;
        parse_hour_minute(configList[C_GO_SLEEP].value, &sleep_hour, &sleep_minute);
        parse_hour_minute(configList[C_GO_WEAK].value, &weak_hour, &weak_minute);
    }

    if (!start_mqtt_client(client_id, addr, port, username, password))
        close_all(EXIT_FAILURE);

    if(!apa102_spi_setup())
        close_all(EXIT_FAILURE);

    fprintf(stdout, "[Info] Initilisation Done! \n");
    fprintf(stdout, "[Info] Client Id ........... %s\n", client_id);
    fprintf(stdout, "[Info] Program ............. %s\n", argv[0]);
    fprintf(stdout, "[Info] LED Number .......... %d\n", leds.numLEDs);
    fprintf(stdout, "[Info] Brightness .......... %d\n", leds.brightness);
    fprintf(stdout, "[Info] Device .............. %s\n", configList[C_MODEL].value);
    fprintf(stdout, "[Info] Nightmode ........... %s\n", flag_sleepmode ? "Enabled": "Disabled");
    fprintf(stdout, "[Info] MQTT Bus ............ %s:%s \n", addr, port);
    fprintf(stdout, "[Info] Press CTRL-C to exit.\n\n");

    while(1){
        if(flag_sleepmode)
            check_nightmode();

        if (flag_update)
            state_machine_update();

        if (flag_terminate)
            break;

        usleep(10000);
    }

    // clean
    close_all(EXIT_SUCCESS);
    return 0;
}

uint32_t text_to_colour(const char* cTxt) {
    if (strlen(cTxt) != 0) {
        if (strcmp(cTxt, "red") == 0) {
            return RED_C;
        } else if (strcmp(cTxt, "green") == 0) {
            return GREEN_C;
        } else if (strcmp(cTxt, "blue") == 0) {
            return BLUE_C;
        } else if (strcmp(cTxt, "yellow") == 0) {
            return YELLOW_C;
        } else if (strcmp(cTxt, "purple") == 0) {
            return PURPLE_C;
        } else if (strcmp(cTxt, "teal") == 0) {
            return TEAL_C;
        } else if (strcmp(cTxt, "orange") == 0) {
            return ORANGE_C;
        }
    }
    return 0;
}

void get_action_colours() {
    uint32_t idle_c = text_to_colour(configList[C_IDLE_COLOUR].value);
    if (idle_c != 0)
        action_colours.idle = idle_c;
    uint32_t listen_c = text_to_colour(configList[C_LISTEN_COLOUR].value);
    if (listen_c != 0)
        action_colours.listen = listen_c;
    uint32_t speak_c = text_to_colour(configList[C_SPEAK_COLOUR].value);
    if (speak_c != 0)
        action_colours.speak = speak_c;
    uint32_t mute_c = text_to_colour(configList[C_MUTE_COLOUR].value);
    if (mute_c != 0)
        action_colours.mute = mute_c;
    uint32_t unmute_c = text_to_colour(configList[C_UNMUTE_COLOUR].value);
    if (unmute_c != 0)
        action_colours.unmute = unmute_c;
}

void check_nightmode(void){
    time_t curr_time;
    struct tm *read_time = NULL;

    curr_time = time(NULL);
    read_time = localtime(&curr_time);

    if(read_time->tm_hour == sleep_hour &&
        read_time->tm_min == sleep_minute &&
        curr_state != ON_DISABLED){
        curr_state = ON_DISABLED;
        flag_update = 1;
        fprintf(stdout, "[Info] ------>  Nightmode started\n");
    }
    if(read_time->tm_hour == weak_hour &&
        read_time->tm_min == weak_minute &&
        curr_state == ON_DISABLED){
        curr_state = ON_IDLE;
        flag_update = 1;
        fprintf(stdout, "[Info] ------>  Nightmode terminated\n");
    }
}

char *generate_client_id(void){
    int i ,seed;
    static char id[CLIENT_ID_LEN + 1] = {0};
    srand(time(NULL));
    for (i = 0; i < CLIENT_ID_LEN; i++){
        seed = rand()%3;
        switch(seed){
            case 0:
                id[i] = rand()%26 + 'a';
                break;
            case 1:
                id[i] = rand()%26 + 'A';
                break;
            case 2:
                id[i] = rand()%10 + '0';
                break;
        }
    }
    return id;
}

void close_all(int status){
    int fd_gpio;
    char gpio_66[]={'6','6'};

    clear();
    if(if_config_true("model", configList, "rsp_corev2")){
        fd_gpio = open("/sys/class/gpio/unexport", O_RDWR);
        if(write(fd_gpio, gpio_66, sizeof(gpio_66))){
            fprintf(stdout, "[Info] Closed GPIO66..\n");
            close(fd_gpio);
        }
    }

    terminate_mqtt_client();
    if (leds.fd_spi != -1) close(leds.fd_spi);
    if (leds.pixels) free(leds.pixels);
    pthread_cancel(curr_thread);
    exit(status);
}

static void interrupt_handler(int sig){
    flag_terminate = 1;
}

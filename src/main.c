#include "apa102.h"
#include "state_handler.h"
#include "mqtt_client.h"
#include "load_hw.h"
#include "parse_opts.h"
#include "cCONFIG.h"

#include <mqtt.h>
#include <common.h>
#include <pthread.h>

#define CLIENT_ID_LEN 10

static void interrupt_handler(int sig);

void check_nightmode(void);
char *generate_client_id(void);
void close_all(int status);
void get_action_colours();

int run_para_init(void);
uint32_t text_to_colour(const char* cTxt);
void debug_run_para_dump(void);
int parse_hour_minute(const char *raw_value, uint8_t *hour, uint8_t *minute);

uint8_t     sleep_hour;
uint8_t     sleep_minute;
uint8_t     weak_hour;
uint8_t     weak_minute;

SNIPS_RUN_PARA RUN_PARA = {
    /* Hardware */
    "",
    /* Brightness */
    31,
    /* MQTT connection */
    "localhost",
    "1883",
    "",
    "",
    /* SiteId */
    "default",
    /* Client ID */
    NULL,
    /* Animation thread */
    0, // NULL
    ON_IDLE,
    /* Colour */
    {GREEN_C, BLUE_C, PURPLE_C, YELLOW_C, GREEN_C},
    /* Sleep mode */
    0,
    0,
    0,
    0,
    /* Flags */
    0,
    1,
    0,
    /* Animation Enable */
    {1, 1, 1, 1, 1}
};

int main(int argc, char *argv[]){

    if ( -1 == run_para_init() )
        close_all(EXIT_FAILURE);

    debug_run_para_dump();

    parse_opts(argc, argv);

    RUN_PARA.client_id = generate_client_id();

    signal(SIGINT, interrupt_handler);

    /*<Test Loading LED info from hw_spec.json file>*/
    load_hw_spec_json(RUN_PARA.hardware_model);

    if(-1 == set_power_pin())
        close_all(EXIT_FAILURE);

    // get brightness
    set_leds_brightness(RUN_PARA.max_brightness);

    if (!start_mqtt_client(RUN_PARA.client_id,
                           RUN_PARA.mqtt_host,
                           RUN_PARA.mqtt_port,
                           RUN_PARA.mqtt_user,
                           RUN_PARA.mqtt_pass))
        close_all(EXIT_FAILURE);

    if(!apa102_spi_setup())
        close_all(EXIT_FAILURE);

    fprintf(stdout, "[Info] Initilisation Done! \n");
    fprintf(stdout, "[Info] Program ............. %s\n", argv[0]);
    fprintf(stdout, "[Info] Client Id ........... %s\n", RUN_PARA.client_id);
    //fprintf(stdout, "[Info] LED Number .......... %d\n", leds.numLEDs);
    fprintf(stdout, "[Info] Brightness .......... %d\n", RUN_PARA.max_brightness);
    fprintf(stdout, "[Info] Device .............. %s\n", RUN_PARA.hardware_model);
    fprintf(stdout, "[Info] Nightmode ........... %s\n", RUN_PARA.if_sleepmode ? "Enabled": "Disabled");
    fprintf(stdout, "[Info] MQTT Bus ............ %s:%s \n", RUN_PARA.mqtt_host, RUN_PARA.mqtt_port);
    fprintf(stdout, "[Info] Press CTRL-C to exit.\n\n");

    while(1){
        if(RUN_PARA.if_sleepmode)
            check_nightmode();

        if (RUN_PARA.if_update)
            state_machine_update();

        if (RUN_PARA.if_terminate)
            break;

        usleep(10000);
    }

    close_all(EXIT_SUCCESS);
    return 0;
}

int run_para_init(void){
    const char *temp = NULL;
    int res;
    res = cCONFIG_Parse_Config(CONFIG_FILE);
    if (-1 == res)
        return -1;

    temp = cCONFIG_Value_Raw(C_MODEL_STR);
    if (temp)
        strcpy(RUN_PARA.hardware_model, temp);

    temp = cCONFIG_Value_Raw(C_LED_BRI_STR);
    if (temp)
        RUN_PARA.max_brightness = atoi(temp);

    temp = cCONFIG_Value_Raw(C_MQTT_HOST_STR);
    if (temp)
        strcpy(RUN_PARA.mqtt_host, temp);

    temp = cCONFIG_Value_Raw(C_MQTT_PORT_STR);
    if (temp)
        strcpy(RUN_PARA.mqtt_port, temp);

    temp = cCONFIG_Value_Raw(C_MQTT_USER_STR);
    if (temp)
        strcpy(RUN_PARA.mqtt_user, temp);

    temp = cCONFIG_Value_Raw(C_MQTT_PASS_STR);
    if (temp)
        strcpy(RUN_PARA.mqtt_pass, temp);

    temp = cCONFIG_Value_Raw(C_SITE_ID_STR);
    if (temp)
        strcpy(RUN_PARA.snips_site_id, temp);

    /* color */
    temp = cCONFIG_Value_Raw(C_IDLE_COLOUR_STR);
    if (temp)
        RUN_PARA.animation_color.idle = text_to_colour(temp);

    temp = cCONFIG_Value_Raw(C_LISTEN_COLOUR_STR);
    if (temp)
        RUN_PARA.animation_color.listen = text_to_colour(temp);

    temp = cCONFIG_Value_Raw(C_SPEAK_COLOUR_STR);
    if (temp)
        RUN_PARA.animation_color.speak = text_to_colour(temp);

    temp = cCONFIG_Value_Raw(C_MUTE_COLOUR_STR);
    if (temp)
        RUN_PARA.animation_color.mute = text_to_colour(temp);

    temp = cCONFIG_Value_Raw(C_UNMUTE_COLOUR_STR);
    if (temp)
        RUN_PARA.animation_color.unmute = text_to_colour(temp);

    /* Animations */
    if (!cCONFIG_Value_Is_True(C_ON_IDLE_STR))
        RUN_PARA.animation_enable[ON_IDLE] = 0;
    if (!cCONFIG_Value_Is_True(C_ON_LISTEN_STR))
        RUN_PARA.animation_enable[ON_LISTEN] = 0;
    if (!cCONFIG_Value_Is_True(C_ON_SPEAK_STR))
        RUN_PARA.animation_enable[ON_SPEAK] = 0;
    if (!cCONFIG_Value_Is_True(C_TO_MUTE_STR))
        RUN_PARA.animation_enable[TO_MUTE] = 0;
    if (!cCONFIG_Value_Is_True(C_TO_UNMUTE_STR))
        RUN_PARA.animation_enable[TO_UNMUTE] = 0;

    /* Sleep mode */
    if (cCONFIG_Value_Is_True(C_NIGHTMODE_STR)){
        RUN_PARA.if_sleepmode = 1;
        temp = cCONFIG_Value_Raw(C_GO_SLEEP_STR);
        parse_hour_minute(temp, &RUN_PARA.sleep_hour, &RUN_PARA.sleep_minute);
        temp = cCONFIG_Value_Raw(C_GO_WEAK_STR);
        parse_hour_minute(temp, &RUN_PARA.weak_hour, &RUN_PARA.weak_minute);
    }

    cCONFIG_Delete_List();
    return 0;
}

void debug_run_para_dump(void){
    /* Hardware */
    printf("hardware_model : %s\n", RUN_PARA.hardware_model);

    /* Brightness */
    printf("max_brightness : %d\n", RUN_PARA.max_brightness);

    /* MQTT connection */
    printf("mqtt_host : %s\n", RUN_PARA.mqtt_host);
    printf("mqtt_port : %s\n", RUN_PARA.mqtt_port);
    printf("mqtt_user : %s\n", RUN_PARA.mqtt_user);
    printf("mqtt_pass : %s\n", RUN_PARA.mqtt_pass);

    /* SiteId */
    printf("site_id : %s\n", RUN_PARA.snips_site_id);

    /* Client ID */
    printf("client_id : %s\n", RUN_PARA.client_id);

    /* Colour */
    printf("idle color :   %8x\n", RUN_PARA.animation_color.idle);
    printf("speak color :  %8x\n", RUN_PARA.animation_color.speak);
    printf("listen color : %8x\n", RUN_PARA.animation_color.listen);
    printf("mute color :   %8x\n", RUN_PARA.animation_color.mute);
    printf("unmute color : %8x\n", RUN_PARA.animation_color.unmute);

    /* Animation Enable */
    printf("on_idle :   %s\n", RUN_PARA.animation_enable[ON_IDLE] ? "true" : "false");
    printf("on_listen : %s\n", RUN_PARA.animation_enable[ON_LISTEN] ? "true" : "false");
    printf("on_speak :  %s\n", RUN_PARA.animation_enable[ON_SPEAK] ? "true" : "false");
    printf("to_mute :   %s\n", RUN_PARA.animation_enable[TO_MUTE] ? "true" : "false");
    printf("to_unmute : %s\n", RUN_PARA.animation_enable[TO_UNMUTE] ? "true" : "false");
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

void check_nightmode(void){
    time_t curr_time;
    struct tm *read_time = NULL;

    curr_time = time(NULL);
    read_time = localtime(&curr_time);

    if(read_time->tm_hour == sleep_hour &&
        read_time->tm_min == sleep_minute &&
        RUN_PARA.curr_state != ON_DISABLED){
        RUN_PARA.curr_state = ON_DISABLED;
        RUN_PARA.if_update = 1;
        fprintf(stdout, "[Info] ------>  Nightmode started\n");
    }
    if(read_time->tm_hour == weak_hour &&
        read_time->tm_min == weak_minute &&
        RUN_PARA.curr_state == ON_DISABLED){
        RUN_PARA.curr_state = ON_IDLE;
        RUN_PARA.if_update = 1;
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

int parse_hour_minute(const char *raw_value, uint8_t *hour, uint8_t *minute){
    char *p;
    char h[3]="";
    char m[3]="";

    p = strchr(raw_value, ':');
    if (p != NULL){
        strncpy(h, raw_value, p - raw_value);
        strcpy(m, p+1);
        *hour = atoi(h);
        *minute = atoi(m);
        return 1;
    }
    return 0;
}

void close_all(int status){
    reset_power_pin();

    terminate_mqtt_client();
    terminate_spi();
    pthread_cancel(RUN_PARA.curr_thread);
    exit(status);
}

static void interrupt_handler(int sig){
    RUN_PARA.if_terminate = 1;
}

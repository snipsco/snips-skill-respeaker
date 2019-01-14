#include "load_sw.h"
#include "cCONFIG.h"
#include "verbose.h"

extern SNIPS_RUN_PARA RUN_PARA;

static uint32_t text_to_colour(const char* cTxt) {
    if (strlen(cTxt)) {
        if (!strcmp(cTxt, "red")) {
            return RED_C;
        } else if (!strcmp(cTxt, "green")) {
            return GREEN_C;
        } else if (!strcmp(cTxt, "blue")) {
            return BLUE_C;
        } else if (!strcmp(cTxt, "yellow")) {
            return YELLOW_C;
        } else if (!strcmp(cTxt, "purple")) {
            return PURPLE_C;
        } else if (!strcmp(cTxt, "teal")) {
            return TEAL_C;
        } else if (!strcmp(cTxt, "orange")) {
            return ORANGE_C;
        }
    }
    return 0;
}

static char *generate_client_id(void){
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

static int parse_hour_minute(const char *raw_value, uint8_t *hour, uint8_t *minute){
    char *p;
    char h[3]="";
    char m[3]="";

    p = strchr(raw_value, ':');
    if (p){
        strncpy(h, raw_value, p - raw_value);
        strcpy(m, p+1);
        *hour = atoi(h);
        *minute = atoi(m);
        return 1;
    }
    return 0;
}

int load_sw_spec(void){
    const char *temp = NULL;
    int res;

    RUN_PARA.client_id = generate_client_id();

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
        parse_hour_minute(temp, &RUN_PARA.wake_hour, &RUN_PARA.wake_minute);
    }

    if (1 == cCONFIG_Value_Is_True(C_FEEDBACK_SOUND_STR))
        RUN_PARA.if_mute = 0;
    else if ( 0 == cCONFIG_Value_Is_True(C_FEEDBACK_SOUND_STR) ) {
        RUN_PARA.if_mute = 1;
    }else
        verbose(VVV_DEBUG, stdout, "Feedback sound info not found!");

    cCONFIG_Delete_List();
    return 0;
}

void debug_run_para_dump(void){
    /* Hardware */
    verbose(VVV_DEBUG, stdout, "hardware_model : %s", RUN_PARA.hardware_model);

    verbose(VVV_DEBUG, stdout, "LED number :  %d", RUN_PARA.LEDs.number);
    verbose(VVV_DEBUG, stdout, "LED SPI BUS : %d", RUN_PARA.LEDs.spi_bus);
    verbose(VVV_DEBUG, stdout, "LED SPI DEV : %d", RUN_PARA.LEDs.spi_dev);
    if (-1 != RUN_PARA.power.pin) {
        verbose(VVV_DEBUG, stdout, "Power PIN :   %d", RUN_PARA.power.pin);
        verbose(VVV_DEBUG, stdout, "Power VAL :   %d", RUN_PARA.power.val);
    }
    if (-1 != RUN_PARA.button.pin) {
        verbose(VVV_DEBUG, stdout, "Button PIN :   %d", RUN_PARA.button.pin);
        verbose(VVV_DEBUG, stdout, "Button VAL :   %d", RUN_PARA.button.val);
    }

    /* Brightness */
    verbose(VVV_DEBUG, stdout, "max_brightness : %d", RUN_PARA.max_brightness);

    /* MQTT connection */
    verbose(VVV_DEBUG, stdout, "mqtt_host : %s", RUN_PARA.mqtt_host);
    verbose(VVV_DEBUG, stdout, "mqtt_port : %s", RUN_PARA.mqtt_port);
    if (0 != RUN_PARA.mqtt_user || 0 != RUN_PARA.mqtt_pass) {
        verbose(VVV_DEBUG, stdout, "mqtt_user : %s", RUN_PARA.mqtt_user);
        verbose(VVV_DEBUG, stdout, "mqtt_pass : %s", RUN_PARA.mqtt_pass);
    }

    /* SiteId */
    verbose(VVV_DEBUG, stdout, "site_id : %s", RUN_PARA.snips_site_id);

    /* Client ID */
    verbose(VVV_DEBUG, stdout, "client_id : %s", RUN_PARA.client_id);

    /* Colour */
    verbose(VVV_DEBUG, stdout, "idle color :   0x%08x", RUN_PARA.animation_color.idle);
    verbose(VVV_DEBUG, stdout, "speak color :  0x%08x", RUN_PARA.animation_color.speak);
    verbose(VVV_DEBUG, stdout, "listen color : 0x%08x", RUN_PARA.animation_color.listen);
    verbose(VVV_DEBUG, stdout, "mute color :   0x%08x", RUN_PARA.animation_color.mute);
    verbose(VVV_DEBUG, stdout, "unmute color : 0x%08x", RUN_PARA.animation_color.unmute);

    /* Animation Enable */
    verbose(VVV_DEBUG, stdout, "on_idle :   %s", RUN_PARA.animation_enable[ON_IDLE] ? "true" : "false");
    verbose(VVV_DEBUG, stdout, "on_listen : %s", RUN_PARA.animation_enable[ON_LISTEN] ? "true" : "false");
    verbose(VVV_DEBUG, stdout, "on_speak :  %s", RUN_PARA.animation_enable[ON_SPEAK] ? "true" : "false");
    verbose(VVV_DEBUG, stdout, "to_mute :   %s", RUN_PARA.animation_enable[TO_MUTE] ? "true" : "false");
    verbose(VVV_DEBUG, stdout, "to_unmute : %s", RUN_PARA.animation_enable[TO_UNMUTE] ? "true" : "false");

    /* Feedback sound */
    verbose(VVV_DEBUG, stdout, "if_mute_feedback sound : %s", RUN_PARA.if_mute ? "YES" : "NO");
}

void dump_running_info(void){
    verbose(VV_INFO, stdout, "Initilisation Done!");
    verbose(VV_INFO, stdout, "Version ............. %s", VERSION);
    verbose(VV_INFO, stdout, "Client Id ........... %s", RUN_PARA.client_id);
    verbose(VV_INFO, stdout, "Brightness .......... %d", RUN_PARA.max_brightness);
    verbose(VV_INFO, stdout, "Device .............. %s", RUN_PARA.hardware_model);
    verbose(VV_INFO, stdout, "Nightmode ........... %s", RUN_PARA.if_sleepmode ? "Enabled": "Disabled");
    verbose(VV_INFO, stdout, "Feedback Sound ...... %s", !RUN_PARA.if_mute ? "Enabled": "Disabled");
    verbose(VV_INFO, stdout, "MQTT Bus ............ %s:%s", RUN_PARA.mqtt_host, RUN_PARA.mqtt_port);

    if (-1 != RUN_PARA.button.pin) {
        verbose(VV_INFO, stdout, "Button GPIO ......... %d", RUN_PARA.button.pin);
        verbose(VV_INFO, stdout, "[Tips] Hit the button to start a new conversion. ");
        verbose(VV_INFO, stdout, "[Tips] Hold the button to switch between mute/unmute the feedback sound ");
    }
    verbose(VV_INFO, stdout, "Press CTRL-C to exit.");
}
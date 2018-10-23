#include "state_handler.h"

extern short flag_update;
extern STATE curr_state;

static void on_idle_handler(const char *topic);
static void on_listen_handler(const char *topic);
static void on_speak_handler(const char *topic);
static void to_mute_handler(const char *topic);
static void to_unmute_handler(const char *topic);
static void on_disabled_handler(const char *topic);

void (*state_handlers[6])(const char *)={ 
    on_idle_handler, 	//0
    on_listen_handler,	//1
    on_speak_handler,	//2
    to_mute_handler, 	//3
    to_unmute_handler,	//4
    on_disabled_handler	//5
};

void state_handler_main(const char *topic){
	STATE last_state = curr_state;
	state_handlers[curr_state](topic);
	if (curr_state != last_state)
		flag_update = 1;
}

static void on_idle_handler(const char *topic){
	if (!strcmp(topic, STA_LIS))        
        curr_state = ON_LISTEN;
    else if (!strcmp(topic, SUD_OFF))
        curr_state = TO_MUTE;
    else if (!strcmp(topic, SUD_ON))
        curr_state = TO_UNMUTE;
    else if (!strcmp(topic, STA_SAY))
        curr_state = ON_SPEAK;
    else if (!strcmp(topic, LED_OFF))
        curr_state = ON_DISABLED;
}

static void on_listen_handler(const char *topic){
	if (!strcmp(topic, END_LIS))
        curr_state = ON_IDLE;
    else if (!strcmp(topic, HOT_ON))
        curr_state = ON_IDLE;
}

static void on_speak_handler(const char *topic){
	if (!strcmp(topic, END_SAY))
        curr_state = ON_IDLE;
    else if (!strcmp(topic, STA_LIS))
        curr_state = ON_LISTEN;
    else if (!strcmp(topic, HOT_ON))
        curr_state = ON_IDLE;
}

static void to_mute_handler(const char *topic){
	if (!strcmp(topic, HOT_ON))
        curr_state = ON_LISTEN;
}

static void to_unmute_handler(const char *topic){
	if (!strcmp(topic, HOT_ON))
        curr_state = ON_LISTEN;
}

static void on_disabled_handler(const char *topic){
	if (!strcmp(topic, LED_ON))
        curr_state = ON_IDLE;
}
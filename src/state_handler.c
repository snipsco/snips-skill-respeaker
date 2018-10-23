#include "state_handler.h"
#include "animation.h"
#include "get_config.h"

extern short 			flag_update;
extern STATE 			curr_state;
extern pthread_t 		curr_thread;
extern snipsSkillConfig configList[CONFIG_NUM];

static void on_idle_handler(const char *topic);
static void on_listen_handler(const char *topic);
static void on_speak_handler(const char *topic);
static void to_mute_handler(const char *topic);
static void to_unmute_handler(const char *topic);
static void on_disabled_handler(const char *topic);

static void* (*state_functions[STATE_NUM])()={ 
    on_idle, 
    on_listen,
    on_speak, 
    to_mute, 
    to_unmute,
    on_disabled
};

static void (*state_handlers[STATE_NUM])(const char *)={ 
    on_idle_handler, 	//0
    on_listen_handler,	//1
    on_speak_handler,	//2
    to_mute_handler, 	//3
    to_unmute_handler,	//4
    on_disabled_handler	//5
};

const char* state_strings[STATE_NUM]={
    ON_IDLE_STR,      //0
    ON_LISTEN_STR,    //1
    ON_SPEAK_STR,     //2
    TO_MUTE_STR,      //3
    TO_UNMUTE_STR,    //4
    ON_DISABLED_STR   //5
};

void state_machine_update(void){
    void *ret_val="NONE";

    if (if_config_true(state_strings[curr_state], configList, NULL) == 1){
        printf("[Debug] State is changed to %d\n", curr_state);
        // block until the previous terminate
        pthread_join(curr_thread,&ret_val);
        printf("[Debug] Previous thread %s terminated with success\n",(char*)ret_val);
        pthread_create(&curr_thread, NULL, state_functions[curr_state], NULL);
    }else{
        flag_update = 0;
    }
}

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
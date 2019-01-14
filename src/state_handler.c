#include "animation.h"
#include "state_handler.h"
#include "verbose.h"

extern SNIPS_RUN_PARA       RUN_PARA;

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
    on_idle_handler,      //0
    on_listen_handler,    //1
    on_speak_handler,     //2
    to_mute_handler,      //3
    to_unmute_handler,    //4
    on_disabled_handler   //5
};

void state_machine_update(void){
    void *ret_val="NONE";

    if (RUN_PARA.animation_enable[RUN_PARA.curr_state]) {
        verbose(VVV_DEBUG, stdout, "State is changed to %d", RUN_PARA.curr_state);
        // block until the previous terminate
        pthread_join(RUN_PARA.curr_thread,&ret_val);
        verbose(VVV_DEBUG, stdout, "Previous thread "PURPLE"%s"NONE" terminated with success", (char*)ret_val);
        pthread_create(&RUN_PARA.curr_thread, NULL, state_functions[RUN_PARA.curr_state], NULL);
    }else{
        RUN_PARA.if_update = 0;
    }
}

void state_handler_main(const char *topic){
    STATE last_state = RUN_PARA.curr_state;
    state_handlers[RUN_PARA.curr_state](topic);
    if (RUN_PARA.curr_state != last_state)
        RUN_PARA.if_update = 1;
}

static void on_idle_handler(const char *topic){
    if (!strcmp(topic, STA_LIS))
        RUN_PARA.curr_state = ON_LISTEN;
    else if (!strcmp(topic, SUD_OFF))
        RUN_PARA.curr_state = TO_MUTE;
    else if (!strcmp(topic, SUD_ON))
        RUN_PARA.curr_state = TO_UNMUTE;
    else if (!strcmp(topic, STA_SAY))
        RUN_PARA.curr_state = ON_SPEAK;
    else if (!strcmp(topic, LED_OFF))
        RUN_PARA.curr_state = ON_DISABLED;
}

static void on_listen_handler(const char *topic){
    if (!strcmp(topic, END_LIS))
        RUN_PARA.curr_state = ON_IDLE;
    else if (!strcmp(topic, HOT_ON))
        RUN_PARA.curr_state = ON_IDLE;
}

static void on_speak_handler(const char *topic){
    if (!strcmp(topic, END_SAY))
        RUN_PARA.curr_state = ON_IDLE;
    else if (!strcmp(topic, STA_LIS))
        RUN_PARA.curr_state = ON_LISTEN;
    else if (!strcmp(topic, HOT_ON))
        RUN_PARA.curr_state = ON_IDLE;
}

static void to_mute_handler(const char *topic){
    if (!strcmp(topic, STA_LIS))
        RUN_PARA.curr_state = ON_LISTEN;
    else if (!strcmp(topic, HOT_ON))
        RUN_PARA.curr_state = ON_IDLE;
}

static void to_unmute_handler(const char *topic){
    if (!strcmp(topic, STA_LIS))
        RUN_PARA.curr_state = ON_LISTEN;
    else if (!strcmp(topic, HOT_ON))
        RUN_PARA.curr_state = ON_IDLE;
}

static void on_disabled_handler(const char *topic){
    if (!strcmp(topic, LED_ON))
        RUN_PARA.curr_state = ON_IDLE;
}

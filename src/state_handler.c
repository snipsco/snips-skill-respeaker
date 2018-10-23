#include "state_handler.h"

extern short        flag_update;
extern pthread_t    curr_thread;

void (*state_handlers[6])(const char *)={ 
    on_idle_handler, 	//0
    on_listen_handler,	//1
    on_speak_handler,	//2
    to_mute_handler, 	//3
    to_unmute_handler,	//4
    on_disabled			//5
};

void state_handler(){
	state_handlers[curr_state]();

}

static void on_idle_handler(){

}

static void on_listen_handler(){

}

static void on_speak_handler(){

}

static void to_mute_handler(){

}

static void to_unmute_handler(){

}

static void on_disabled_handler(){

}
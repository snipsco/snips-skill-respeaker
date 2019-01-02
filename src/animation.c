#include "animation.h"
#include "apa102.h"
#include "log.h"

extern APA102       leds;
extern SNIPS_RUN_PARA RUN_PARA;

// 0
void *on_idle(){
    int curr_bri = 0;
    uint8_t led, step;
    fprintf(stdout, "[Animation] ------>  " BLUE " %s " NONE " started\n", __FUNCTION__);
    RUN_PARA.if_update = 0;
    clear();
    srand((unsigned int)time(NULL));

    step = leds.brightness / STEP_COUNT;
    while(RUN_PARA.curr_state == ON_IDLE){
        delay_on_state(2000, ON_IDLE);
        clear();
        led = rand()%leds.numLEDs;

        for (curr_bri = 0;
            curr_bri < leds.brightness &&
            RUN_PARA.curr_state == ON_IDLE;
            curr_bri += step){
            set_index_4byte(led, RUN_PARA.animation_color.idle | curr_bri);
            show();
            delay_on_state(100, ON_IDLE);
        }
        curr_bri = leds.brightness;
        for (curr_bri = leds.brightness;
            curr_bri > 0 &&
            RUN_PARA.curr_state == ON_IDLE;
            curr_bri -= step){
            set_index_4byte(led, RUN_PARA.animation_color.idle | curr_bri);
            show();
            delay_on_state(100, ON_IDLE);
        }
        set_index_rgb(led, 0, 0, 0);
        show();
        delay_on_state(3000, ON_IDLE);
    }
    clear();
    return((void *)"ON_IDLE");
}

// 1
void *on_listen(){
    uint8_t i,g,group;
    fprintf(stdout, "[Animation] ------>  " BLUE " %s " NONE " started\n", __FUNCTION__);
    RUN_PARA.if_update = 0;
    clear();
    group = leds.numLEDs/3;
    while(RUN_PARA.curr_state == ON_LISTEN){
        for(i=0;i<3 && RUN_PARA.curr_state == ON_LISTEN; i++){
            for (g=0; g < group && RUN_PARA.curr_state == ON_LISTEN; g++)
                set_index_4byte(g*3+i, RUN_PARA.animation_color.listen | leds.brightness);
            show();
            delay_on_state(80, ON_LISTEN);
            clear();
            delay_on_state(80, ON_LISTEN);
        }
    }
    clear();
    return((void *)"ON_LISTEN");
}

// 2
void *on_speak(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    fprintf(stdout, "[Animation] ------>  " BLUE " %s " NONE " started\n", __FUNCTION__);
    RUN_PARA.if_update = 0;
    clear();

    step = leds.brightness / STEP_COUNT;
    while(RUN_PARA.curr_state == ON_SPEAK){
        for (curr_bri = 0;
            curr_bri < leds.brightness &&
            RUN_PARA.curr_state == ON_SPEAK;
            curr_bri += step){
            for (j = 0; j < leds.numLEDs && RUN_PARA.curr_state == ON_SPEAK; j++)
                set_index_4byte(j, RUN_PARA.animation_color.speak | curr_bri);
            show();
            delay_on_state(20, ON_SPEAK);
        }
        curr_bri = leds.brightness;
        for (curr_bri = leds.brightness;
            curr_bri > 0 &&
            RUN_PARA.curr_state == ON_SPEAK;
            curr_bri -= step){
            for (j = 0; j < leds.numLEDs && RUN_PARA.curr_state == ON_SPEAK; j++)
                set_index_4byte(j, RUN_PARA.animation_color.speak | curr_bri);
            show();
            delay_on_state(20, ON_SPEAK);
        }
        for (j = 0; j < leds.numLEDs && RUN_PARA.curr_state == ON_SPEAK; j++)
            set_index_rgb(j, 0, 0, 0);
        show();
        delay_on_state(200, ON_SPEAK);
    }
    clear();
    return((void *)"ON_SPEAK");
}

// 3
void *to_mute(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    fprintf(stdout, "[Animation] ------>  " BLUE " %s " NONE " started\n", __FUNCTION__);
    RUN_PARA.if_update = 0;
    clear();

    step = leds.brightness / STEP_COUNT;
    for (curr_bri = 0; curr_bri < leds.brightness && RUN_PARA.curr_state == TO_MUTE; curr_bri += step){
        for (j = 0; j < leds.numLEDs && RUN_PARA.curr_state == TO_MUTE; j++)
            set_index_4byte(j, RUN_PARA.animation_color.mute | curr_bri);
        show();
        delay_on_state(50, TO_MUTE);
    }
    curr_bri = leds.brightness;
    for (curr_bri = leds.brightness; curr_bri > 0 && RUN_PARA.curr_state == TO_MUTE; curr_bri -= step){
        for (j = 0; j < leds.numLEDs && RUN_PARA.curr_state == TO_MUTE; j++)
            set_index_4byte(j, RUN_PARA.animation_color.mute | curr_bri);
        show();
        delay_on_state(50, TO_MUTE);
    }
    for (j = 0; j < leds.numLEDs && RUN_PARA.curr_state == TO_MUTE; j++)
        set_index_rgb(j, 0, 0, 0);
    show();
    if(RUN_PARA.curr_state == TO_MUTE){
        RUN_PARA.curr_state = ON_IDLE;
        RUN_PARA.if_update = 1;
    }
    clear();
    return((void *)"TO_MUTE");
}

// 4
void *to_unmute(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    fprintf(stdout, "[Animation] ------>  " BLUE " %s " NONE " started\n", __FUNCTION__);
    RUN_PARA.if_update = 0;
    clear();

    step = leds.brightness / STEP_COUNT;
    for (curr_bri = 0;curr_bri < leds.brightness && RUN_PARA.curr_state == TO_UNMUTE; curr_bri += step){
        for (j = 0; j < leds.numLEDs && RUN_PARA.curr_state == TO_UNMUTE; j++)
            set_index_4byte(j, RUN_PARA.animation_color.unmute | curr_bri);
        show();
        delay_on_state(50, TO_UNMUTE);
    }
    curr_bri = leds.brightness;
    for (curr_bri = leds.brightness; curr_bri > 0 && RUN_PARA.curr_state == TO_UNMUTE; curr_bri -= step){
        for (j = 0; j < leds.numLEDs && RUN_PARA.curr_state == TO_UNMUTE; j++)
            set_index_4byte(j, RUN_PARA.animation_color.unmute | curr_bri);
        show();
        delay_on_state(50, TO_UNMUTE);
    }
    for (j = 0; j < leds.numLEDs && RUN_PARA.curr_state == TO_UNMUTE; j++)
        set_index_rgb(j, 0, 0, 0);
    show();
    if(RUN_PARA.curr_state == TO_UNMUTE){
        RUN_PARA.curr_state = ON_IDLE;
        RUN_PARA.if_update = 1;
    }
    clear();
    return((void *)"TO_UNMUTE");
}

// 5
void *on_disabled(){
    fprintf(stdout, "[Animation] ------>  " BLUE " %s " NONE " started\n", __FUNCTION__);
    RUN_PARA.if_update = 0;
    while(RUN_PARA.curr_state == ON_DISABLED){
        clear();
        delay_on_state(100, ON_DISABLED);
    }
    clear();
    return((void *)"ON_DISABLED");
}

void delay_on_state(int ms, int state){
    for (int j = 0; j < ms && RUN_PARA.curr_state == state; j++)
        usleep(1000);
}

#include "animation.h"
#include "apa102.h"

extern APA102       leds;
extern short        flag_update;
extern pthread_t    curr_thread;

extern numLEDs;
extern fd_spi;
extern *pixels;

void *on_idle(){
    uint8_t i,j;
    int curr_bri = 0;
    uint8_t led, step;
    printf("[Thread] ------>  on_idle started\n");
    flag_update = 0;
    clear();
    srand((unsigned int)time(NULL));

    step = leds.brightness / STEP_COUNT;
    while(curr_state == ON_IDLE){
        delay_on_state(2000, ON_IDLE);
        clear();
        led = rand()%leds.numLEDs;

        for (curr_bri = 0; 
            curr_bri < leds.brightness && 
            curr_state == ON_IDLE; 
            curr_bri += step){
            set_index_rgb(led, 0, curr_bri, 0);
            show();
            delay_on_state(100, ON_IDLE);
        }
        curr_bri = leds.brightness;
        for (curr_bri = leds.brightness;
            curr_bri > 0 && 
            curr_state == ON_IDLE;
            curr_bri -= step){
            set_index_rgb(led, 0, curr_bri, 0);
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
    printf("[Thread] ------>  on_listen started\n");
    flag_update = 0;
    clear();
    group = leds.numLEDs/3;
    while(curr_state == ON_LISTEN){
        for(i=0;i<3 && curr_state == ON_LISTEN; i++){
            for (g=0; g < group && curr_state == ON_LISTEN; g++)
                set_index_rgb(g*3+i, leds.brightness, 0, 0);
            show();
            delay_on_state(80, ON_LISTEN);
            clear();
            delay_on_state(80, ON_LISTEN);
        }
    }
    clear();
    return((void *)"ON_LISTEN");   
}

// 2 ignore - > too fast to perform
void *on_think(){
    // uint8_t i,g,group;
    // printf("[Thread] ------>  on_think started\n");
    // clear();
    // group = leds.numLEDs/3;
    // while(curr_state == 2){
    //     for(i=3; i>0; i--){
    //         for (g=0; g < group; g++)
    //             set_index_rgb(g*3+i-1, 0, 255, 255);
    //         show();
    //         for (int j = 0; j < 10; j++){
    //             // each 0.01s check
    //             if(curr_state != 2) goto TERMINATE_THREAD;
    //             usleep(10000);
    //         }
    //         clear();
    //         for (int j = 0; j < 10; j++){
    //             // each 0.01s check
    //             if(curr_state != 2) goto TERMINATE_THREAD;
    //             usleep(10000);
    //         }
    //     }
    // }
    // TERMINATE_THREAD:
    // clear();
    // return((void *)"ON_THINK");
}

// 3
void *on_speak(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    printf("[Thread] ------>  on_speak started\n");
    flag_update = 0;
    clear();
    
    step = leds.brightness / STEP_COUNT;
    while(curr_state == ON_SPEAK){
        for (curr_bri = 0;
            curr_bri < leds.brightness &&
            curr_state == ON_SPEAK;
            curr_bri += step){
            for (j = 0; j < leds.numLEDs && curr_state == ON_SPEAK; j++)
                set_index_rgb(j, curr_bri, 0, curr_bri);
            show();
            delay_on_state(20, ON_SPEAK);
        }
        curr_bri = leds.brightness;
        for (curr_bri = leds.brightness;
            curr_bri > 0 &&
            curr_state == ON_SPEAK;
            curr_bri -= step){
            for (j = 0; j < leds.numLEDs && curr_state == ON_SPEAK; j++)
                set_index_rgb(j, curr_bri, 0, curr_bri);
            show();
            delay_on_state(20, ON_SPEAK);
        }
        for (j = 0; j < leds.numLEDs && curr_state == ON_SPEAK; j++)
            set_index_rgb(j, 0, 0, 0);
        show();
        delay_on_state(200, ON_SPEAK);
    }
    clear();
    return((void *)"ON_SPEAK");
}

// 4
void *to_mute(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    printf("[Thread] ------>  to_mute started\n");
    flag_update = 0;
    clear();
    
    step = leds.brightness / STEP_COUNT;
    for (curr_bri = 0; curr_bri < leds.brightness && curr_state == TO_MUTE; curr_bri += step){
        for (j = 0; j < leds.numLEDs && curr_state == TO_MUTE; j++)
            set_index_rgb(j, 0, 0, curr_bri);
        show();
        delay_on_state(100, TO_MUTE);
    }
    curr_bri = leds.brightness;
    for (curr_bri = leds.brightness; curr_bri > 0 && curr_state == TO_MUTE; curr_bri -= step){
        for (j = 0; j < leds.numLEDs && curr_state == TO_MUTE; j++)
            set_index_rgb(j, 0, 0, curr_bri);
        show();
        delay_on_state(100, TO_MUTE);
    }
    for (j = 0; j < leds.numLEDs && curr_state == TO_MUTE; j++)
        set_index_rgb(j, 0, 0, 0);
    show();
    if(curr_state == TO_MUTE){
        curr_state = ON_IDLE;
        flag_update = 0;
    }
    clear();
    return((void *)"TO_MUTE");
}

// 5
void *to_unmute(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    printf("[Thread] ------>  to_unmute started\n");
    flag_update = 0;
    clear();
    
    step = leds.brightness / STEP_COUNT;
    for (curr_bri = 0;curr_bri < leds.brightness && curr_state == TO_UNMUTE; curr_bri += step){
        for (j = 0; j < leds.numLEDs && curr_state == TO_UNMUTE; j++)
            set_index_rgb(j, curr_bri, 0, 0);
        show();
        delay_on_state(100, TO_UNMUTE);
    }
    curr_bri = leds.brightness;
    for (curr_bri = leds.brightness; curr_bri > 0 && curr_state == TO_UNMUTE; curr_bri -= step){
        for (j = 0; j < leds.numLEDs && curr_state == TO_UNMUTE; j++)
            set_index_rgb(j, curr_bri, 0, 0);
        show();
        delay_on_state(100, TO_UNMUTE);
    }
    for (j = 0; j < leds.numLEDs && curr_state == TO_UNMUTE; j++)
        set_index_rgb(j, 0, 0, 0);
    show();
    if(curr_state == TO_UNMUTE){
        curr_state = ON_IDLE;
        flag_update = 0;
    }
    clear();
    return((void *)"TO_UNMUTE");
}

// 6
void *on_success(){
    uint8_t i,g,group;
    printf("[Thread] ------>  on_success started\n");
    flag_update = 0;
    clear();
    group = leds.numLEDs/3;
    while(curr_state == ON_SUCCESS){
        for(i=3; i>0; i--){
            for (g=0; g < group; g++)
                set_index_rgb(g*3+i-1, 0, leds.brightness, 0);
            show();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != ON_SUCCESS) goto TERMINATE_THREAD;
                usleep(10000);
            }
            clear();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != ON_SUCCESS) goto TERMINATE_THREAD;
                usleep(10000);
            }
        }
    }
    TERMINATE_THREAD:
    clear();
    return((void *)"ON_SUCCESS");
}

// 7
void *on_error(){
    uint8_t i,g,group;
    printf("[Thread] ------>  on_error started\n");
    flag_update = 0;
    clear();
    group = leds.numLEDs/3;
    while(curr_state == ON_ERROR){
        for(i=3; i>0 && curr_state == ON_ERROR; i--){
            for (g=0; g < group && curr_state == ON_ERROR; g++)
                set_index_rgb(g*3+i-1, 0, 0, leds.brightness);
            show();
            delay_on_state(80, ON_ERROR);
            clear();
            delay_on_state(80, ON_ERROR);
        }
    }
    clear();
    return((void *)"ON_ERROR");
}

// 8
void *on_disabled(){
    uint8_t j;
    printf("[Thread] ------>  on_disabled started\n");
    flag_update = 0;
    while(curr_state == ON_DISABLED){
        clear();
        delay_on_state(100, ON_DISABLED);
    }
    clear();
    return((void *)"ON_DISABLED");
}

void delay_on_state(int ms, int state){
    for (int j = 0; j < ms && curr_state == state; j++)
        usleep(1000);
}

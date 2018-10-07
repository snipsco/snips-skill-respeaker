#include "animation.h"

extern APA102 leds;
extern short curr_state;

extern numLEDs;
extern fd_spi;
extern *pixels;

void *on_idle(){
    uint8_t i,j;
    uint8_t curr_bri = 0;
    uint8_t led;
    printf("[Thread] ------>  on_idle started\n");
    clear();
    srand((unsigned int)time(NULL));
    while(curr_state == 0){
        
        clear();
        led = rand()%leds.numLEDs;

        for (curr_bri = 0; curr_bri < 101; curr_bri += 5)
        {
            set_index_rgb(led, 0, curr_bri, 0);
            show();
            for (int j = 0; j < 10; j++){
                // each 0.01s check
                if(curr_state != 0) {clear();return((void *)0);}
                usleep(10000);
            }
        }

        for (curr_bri = 100; curr_bri > 0; curr_bri -= 5)
        {
            set_index_rgb(led, 0, curr_bri, 0);
            show();
            for (int j = 0; j < 10; j++){
                // each 0.01s check
                if(curr_state != 0) {clear();return((void *)0);}
                usleep(10000);
            }
        }
        
        // every 5 seconds
        for (int j = 0; j < 500; j++){
            // each 0.05s check
            if(curr_state != 0) {clear();return((void *)0);}
            usleep(10000);
        }
    }
    clear();
    return((void *)0);
}

// 1
void *on_listen(){
    uint8_t i,g,group;
    printf("[Thread] ------>  on_listen started\n");
    clear();
    group = leds.numLEDs/3;
    while(curr_state == 1){
        for(i=0; i<3; i++){
            for (g=0; g < group; g++)
                set_index_rgb(g*3+i, 255, 0, 0);
            show();
            if(curr_state != 1) {clear();return((void *)1);}
            usleep(100000);
            clear();
            if(curr_state != 1) {clear();return((void *)1);}
            usleep(100000);
        }
    }
    clear();
    return((void *)1);   
}

// 2
void *on_think(){
    uint8_t i,g,group;
    printf("[Thread] ------>  on_think started\n");
    clear();
    group = leds.numLEDs/3;
    while(curr_state == 2){
        for(i=2; i>0; i--){
            for (g=0; g < group; g++)
                set_index_rgb(g*3+i, 0, 255, 255);
            show();
            if(curr_state != 2) {clear();return((void *)2);}
            usleep(100000);
            clear();
            if(curr_state != 2) {clear();return((void *)2);}
            usleep(100000);
        }
    }
    clear();
    return((void *)2);
}

// 3
void *on_speak(){
    ;
}

// 4
void *to_mute(){
    ;
}

// 5
void *to_unmute(){
    ;
}

// 7 
void *on_success(){
    ;
}

// 6
void *on_error(){
    ;
}

// 7
void *on_off(){
    ;
}
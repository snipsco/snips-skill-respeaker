#include "animation.h"

extern APA102       leds;
extern short        curr_state;
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
    while(curr_state == 0){
        // wait 2 seconds
        for (int j = 0; j < 200; j++){
            // each 0.01s check
            if(curr_state != 0) goto TERMINATE_THREAD;
            usleep(10000);
        }

        clear();
        led = rand()%leds.numLEDs;

        for (curr_bri = 0; curr_bri < leds.brightness; curr_bri += step){
            set_index_rgb(led, 0, curr_bri, 0);
            show();
            for (int j = 0; j < 10; j++){
                // each 0.01s check
                if(curr_state != 0) goto TERMINATE_THREAD;
                usleep(10000);
            }
        }
        curr_bri = leds.brightness;
        for (curr_bri = leds.brightness; curr_bri > 0; curr_bri -= step){
            set_index_rgb(led, 0, curr_bri, 0);
            show();
            for (int j = 0; j < 10; j++){
                // each 0.01s check
                if(curr_state != 0) goto TERMINATE_THREAD;
                usleep(10000);
            }
        }
        set_index_rgb(led, 0, 0, 0);
        show();
        // wait 3 seconds
        for (int j = 0; j < 300; j++){
            // each 0.01s check
            if(curr_state != 0) goto TERMINATE_THREAD;
            usleep(10000);
        }
    }
    TERMINATE_THREAD:
    clear();
    return((void *)"ON_IDLE");
}

// 1
void *on_listen(){
    uint8_t i,j,g,group;
    printf("[Thread] ------>  on_listen started\n");
    flag_update = 0;
    clear();
    group = leds.numLEDs/3;
    while(curr_state == 1){
        if(curr_state != 1) goto TERMINATE_THREAD;
        for(i=0; i<3; i++){
            if(curr_state != 1) goto TERMINATE_THREAD;
            for (g=0; g < group; g++)
                set_index_rgb(g*3+i, leds.brightness, 0, 0);
            if(curr_state != 1) goto TERMINATE_THREAD;
            show();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 1) goto TERMINATE_THREAD;
                usleep(10000);
            }
            clear();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 1) goto TERMINATE_THREAD;
                usleep(10000);
            }
        }
    }
    TERMINATE_THREAD:
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
    while(curr_state == 3){
        for (curr_bri = 0; curr_bri < leds.brightness; curr_bri += step){
            for (j = 0; j < leds.numLEDs; j++)
                set_index_rgb(j, curr_bri, 0, curr_bri);

            show();
            for (int j = 0; j < 2; j++){
                // each 0.01s check
                if(curr_state != 3) goto TERMINATE_THREAD;
                usleep(10000);
            }
        }
        curr_bri = leds.brightness;
        for (curr_bri = leds.brightness; curr_bri > 0; curr_bri -= step){
            for (j = 0; j < leds.numLEDs; j++)
                set_index_rgb(j, curr_bri, 0, curr_bri);

            show();
            for (int j = 0; j < 2; j++){
                // each 0.01s check
                if(curr_state != 3) goto TERMINATE_THREAD;
                usleep(10000);
            }
        }
        if(curr_state != 3) goto TERMINATE_THREAD;
        for (j = 0; j < leds.numLEDs; j++)
            set_index_rgb(j, 0, 0, 0);
        show();

        for (int j = 0; j < 20; j++){
            // each 0.01s check
            if(curr_state != 3) goto TERMINATE_THREAD;
            usleep(10000);
        }
    }
    TERMINATE_THREAD:
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
    for (curr_bri = 0; curr_bri < leds.brightness; curr_bri += step){
        for (j = 0; j < leds.numLEDs; j++)
            set_index_rgb(j, 0, 0, curr_bri);

        show();
        for (int j = 0; j < 10; j++){
            // each 0.01s check
            if(curr_state != 4) goto TERMINATE_THREAD;
            usleep(10000);
        }
    }
    curr_bri = leds.brightness;
    for (curr_bri = leds.brightness; curr_bri > 0; curr_bri -= step){
        for (j = 0; j < leds.numLEDs; j++)
            set_index_rgb(j, 0, 0, curr_bri);

        show();
        for (int j = 0; j < 10; j++){
            // each 0.01s check
            if(curr_state != 4) goto TERMINATE_THREAD;
            usleep(10000);
        }
    }

    for (j = 0; j < leds.numLEDs; j++)
        set_index_rgb(j, 0, 0, 0);
    show();
    if(curr_state != 4) goto TERMINATE_THREAD;
    curr_state = 0;
    flag_update = 0;
    TERMINATE_THREAD:
    clear();
    return((void *)"TO_MUTE");
}

// 5
void *to_unmute(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    printf("[Thread] ------>  to_mute started\n");
    flag_update = 0;
    clear();
    
    step = leds.brightness / STEP_COUNT;
    for (curr_bri = 0; curr_bri < leds.brightness; curr_bri += step){
        for (j = 0; j < leds.numLEDs; j++)
            set_index_rgb(j, curr_bri, 0, 0);

        show();
        for (int j = 0; j < 10; j++){
            // each 0.01s check
            if(curr_state != 5) goto TERMINATE_THREAD;
            usleep(10000);
        }
    }
    curr_bri = leds.brightness;
    for (curr_bri = leds.brightness; curr_bri > 0; curr_bri -= step){
        for (j = 0; j < leds.numLEDs; j++)
            set_index_rgb(j, curr_bri, 0, 0);

        show();
        for (int j = 0; j < 10; j++){
            // each 0.01s check
            if(curr_state != 5) goto TERMINATE_THREAD;
            usleep(10000);
        }
    }

    for (j = 0; j < leds.numLEDs; j++)
        set_index_rgb(j, 0, 0, 0);
    show();
    if(curr_state != 5) goto TERMINATE_THREAD;
    curr_state = 0;
    flag_update = 0;
    TERMINATE_THREAD:
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
    while(curr_state == 6){
        for(i=3; i>0; i--){
            for (g=0; g < group; g++)
                set_index_rgb(g*3+i-1, 0, leds.brightness, 0);
            show();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 6) goto TERMINATE_THREAD;
                usleep(10000);
            }
            clear();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 6) goto TERMINATE_THREAD;
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
    while(curr_state == 7){
        for(i=3; i>0; i--){
            for (g=0; g < group; g++)
                set_index_rgb(g*3+i-1, 0, 0, leds.brightness);
            show();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 7) goto TERMINATE_THREAD;
                usleep(10000);
            }
            clear();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 7) goto TERMINATE_THREAD;
                usleep(10000);
            }
        }
    }
    TERMINATE_THREAD:
    clear();
    return((void *)"ON_ERROR");
}

// 8
void *on_off(){
    uint8_t j;
    printf("[Thread] ------>  on_off started\n");
    flag_update = 0;
    while(curr_state == 8){
        clear();
        for (int j = 0; j < 10; j++){
            // each 0.01s check
            if(curr_state != 8) goto TERMINATE_THREAD;
            usleep(10000);
        }
    }
    TERMINATE_THREAD:
    clear();
    return((void *)"ON_OFF");
}

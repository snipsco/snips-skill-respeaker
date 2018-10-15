#include "animation.h"

extern APA102       leds;
extern short        curr_state;
extern short        last_state;
extern pthread_t    curr_thread;

extern numLEDs;
extern fd_spi;
extern *pixels;

void *on_idle(){
    uint8_t i,j;
    int curr_bri = 0;
    uint8_t led, step;
    printf("[Thread] ------>  on_idle started\n");
    clear();
    srand((unsigned int)time(NULL));

    step = leds.brightness / STEP_COUNT;
    while(curr_state == 0){
        
        clear();
        led = rand()%leds.numLEDs;

        for (curr_bri = 0; curr_bri < leds.brightness; curr_bri += step){
            set_index_rgb(led, 0, curr_bri, 0);
            show();
            for (int j = 0; j < 10; j++){
                // each 0.01s check
                if(curr_state != 0) {clear();return((void *)0);}
                usleep(10000);
            }
        }
        curr_bri = leds.brightness;
        for (curr_bri = leds.brightness; curr_bri > 0; curr_bri -= step){
            set_index_rgb(led, 0, curr_bri, 0);
            show();
            for (int j = 0; j < 10; j++){
                // each 0.01s check
                if(curr_state != 0) {clear();return((void *)0);}
                usleep(10000);
            }
        }

        set_index_rgb(led, 0, 0, 0);
        show();
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
    uint8_t i,j,g,group;
    printf("[Thread] ------>  on_listen started\n");
    clear();
    group = leds.numLEDs/3;
    while(curr_state == 1){
        if(curr_state != 1) {clear();return((void *)0);}
        for(i=0; i<3; i++){
            if(curr_state != 1) {clear();return((void *)0);}
            for (g=0; g < group; g++)
                set_index_rgb(g*3+i, leds.brightness, 0, 0);
            if(curr_state != 1) {clear();return((void *)0);}
            show();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 1) {clear();return((void *)0);}
                usleep(10000);
            }
            clear();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 1) {clear();return((void *)0);}
                usleep(10000);
            }
        }
    }
    clear();
    return((void *)1);   
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
    //             if(curr_state != 2) {clear();return((void *)0);}
    //             usleep(10000);
    //         }
    //         clear();
    //         for (int j = 0; j < 10; j++){
    //             // each 0.01s check
    //             if(curr_state != 2) {clear();return((void *)0);}
    //             usleep(10000);
    //         }
    //     }
    // }
    // clear();
    // return((void *)2);
}

// 3
void *on_speak(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    printf("[Thread] ------>  on_speak started\n");
    clear();
    
    step = leds.brightness / STEP_COUNT;
    while(curr_state == 3){
        for (curr_bri = 0; curr_bri < leds.brightness; curr_bri += step){
            printf("current brightness is %d\n", curr_bri);
            for (j = 0; j < leds.numLEDs; j++)
                set_index_rgb(j, curr_bri, 0, curr_bri);

            show();
            for (int j = 0; j < 2; j++){
                // each 0.01s check
                if(curr_state != 3) {clear();return((void *)0);}
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
                if(curr_state != 3) {clear();return((void *)0);}
                usleep(10000);
            }
        }
        if(curr_state != 3) {clear();return((void *)0);}
        for (j = 0; j < leds.numLEDs; j++)
            set_index_rgb(j, 0, 0, 0);
        show();

        for (int j = 0; j < 20; j++){
            // each 0.01s check
            if(curr_state != 3) {clear();return((void *)0);}
            usleep(10000);
        }
    }
    clear();
    return((void *)0);
}

// 4
void *to_mute(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    printf("[Thread] ------>  to_mute started\n");
    clear();
    
    step = leds.brightness / STEP_COUNT;
    for (curr_bri = 0; curr_bri < leds.brightness; curr_bri += step){
        for (j = 0; j < leds.numLEDs; j++)
            set_index_rgb(j, 0, 0, curr_bri);

        show();
        for (int j = 0; j < 10; j++){
            // each 0.01s check
            if(curr_state != 4) {clear();return((void *)0);}
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
            if(curr_state != 4) {clear();return((void *)0);}
            usleep(10000);
        }
    }

    for (j = 0; j < leds.numLEDs; j++)
        set_index_rgb(j, 0, 0, 0);
    show();
    if(curr_state != 4) {clear();return((void *)0);}
    last_state = curr_state;
    curr_state = 0;
    pthread_create(&curr_thread, NULL, on_idle, NULL);
    clear();
    return((void *)0);
}

// 5
void *to_unmute(){
    uint8_t j;
    uint8_t step;
    int curr_bri = 0;
    printf("[Thread] ------>  to_mute started\n");
    clear();
    
    step = leds.brightness / STEP_COUNT;
    for (curr_bri = 0; curr_bri < leds.brightness; curr_bri += step){
        for (j = 0; j < leds.numLEDs; j++)
            set_index_rgb(j, curr_bri, 0, 0);

        show();
        for (int j = 0; j < 10; j++){
            // each 0.01s check
            if(curr_state != 5) {clear();return((void *)0);}
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
            if(curr_state != 5) {clear();return((void *)0);}
            usleep(10000);
        }
    }

    for (j = 0; j < leds.numLEDs; j++)
        set_index_rgb(j, 0, 0, 0);
    show();
    if(curr_state != 5) {clear();return((void *)0);}
    last_state = curr_state;
    curr_state = 0;
    pthread_create(&curr_thread, NULL, on_idle, NULL);
    clear();
    return((void *)0);
}

// 6
void *on_success(){
    uint8_t i,g,group;
    printf("[Thread] ------>  on_success started\n");
    clear();
    group = leds.numLEDs/3;
    while(curr_state == 6){
        for(i=3; i>0; i--){
            for (g=0; g < group; g++)
                set_index_rgb(g*3+i-1, 0, leds.brightness, 0);
            show();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 6) {clear();return((void *)0);}
                usleep(10000);
            }
            clear();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 6) {clear();return((void *)0);}
                usleep(10000);
            }
        }
    }
    clear();
    return((void *)2);
}

// 7
void *on_error(){
    uint8_t i,g,group;
    printf("[Thread] ------>  on_error started\n");
    clear();
    group = leds.numLEDs/3;
    while(curr_state == 7){
        for(i=3; i>0; i--){
            for (g=0; g < group; g++)
                set_index_rgb(g*3+i-1, 0, 0, leds.brightness);
            show();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 7) {clear();return((void *)0);}
                usleep(10000);
            }
            clear();
            for (int j = 0; j < 8; j++){
                // each 0.01s check
                if(curr_state != 7) {clear();return((void *)0);}
                usleep(10000);
            }
        }
    }
    clear();
    return((void *)2);
}

// 8
void *on_off(){
    uint8_t j;
    printf("[Thread] ------>  on_off started\n");
    while(curr_state == 8){
        clear();
        for (int j = 0; j < 10; j++){
            // each 0.01s check
            if(curr_state != 8) {clear();return((void *)0);}
            usleep(10000);
        }
    }
    clear();
    return((void *)0);
}

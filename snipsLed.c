#include "include/snipsLed.h"

short       last_state = 0;
short       state_flag = 1;
short       state = 0;
char        *client_id;

uint32_t    numLEDs;
uint32_t    bitrate = 32000000;
int         fd = -1;
int         sockfd = -1;
uint8_t     *pixels = NULL;
uint8_t     rOffset = 1;
uint8_t     gOffset = 2;
uint8_t     bOffset = 3;

pthread_t global_thread[9];

pthread_t curr_thread;
pthread_t last_thread;

void (*f[9])(const char *)={on_idle, on_listen, on_think, on_speak, to_mute, to_unmute, on_success, on_error, on_off};

int main(int argc, const char *argv[]) 
{
    const char* addr;
    const char* port;
    const char* topic[NUM_TOPIC];
    int temp;
    client_id = generate_client_id();
    short i;
    printf("[Info] Client id is: %s\n", client_id);

    /* get address (argv[1] if present) */
    if (argc > 1) {
        addr = argv[1];
    } else {
        addr = "localhost";
    }

    /* get port number (argv[2] if present) */
    if (argc > 2) {
        port = argv[2];
    } else {
        port = "1883";
    }

    if (argc > 3) {
        if (strcmp(argv[3],"3") == 0){
            numLEDs = 3;
        }else if(strcmp(argv[3],"12") == 0){
            numLEDs = 12;
        }
    }else{
        numLEDs = 3;
    }
    
    hw_init();

    topic[0] = "hermes/hotword/toggleOff";
    topic[1] = "hermes/asr/startListening";
    topic[2] = "hermes/asr/stopListening";
    topic[3] = "hermes/tts/say";
    topic[4] = "hermes/tts/sayFinished";
    topic[5] = "hermes/hotword/toggleOn";
    topic[6] = "hermes/feedback/sound/toggleOn";
    topic[7] = "hermes/feedback/sound/toggleOff";
    topic[8] = "hermes/nlu/intentNotRecognized";
    topic[9] = "hermes/nlu/intentParsed";


    /* open the non-blocking TCP socket (connecting to the broker) */
    sockfd = open_nb_socket(addr, port);

    if (sockfd == -1) {
        perror("Failed to open socket: ");
        terminate(EXIT_FAILURE, NULL);
    }

    /* setup a client */
    struct mqtt_client client;
    uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
    uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */
    mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback);
    mqtt_connect(&client, client_id, NULL, NULL, 0, NULL, NULL, 0, 400);

    /* check that we don't have any errors */
    if (client.error != MQTT_OK) {
        fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
        terminate(EXIT_FAILURE, NULL);
    }

    /* start a thread to refresh the client (handle egress and ingree client traffic) */
    pthread_t client_daemon;
    if(pthread_create(&client_daemon, NULL, client_refresher, &client)) {
        fprintf(stderr, "Failed to start client daemon.\n");
        terminate(EXIT_FAILURE, NULL);

    }

    /* subscribe */
    for(i=0;i<NUM_TOPIC;i++){
        mqtt_subscribe(&client, topic[i], 0);
        printf("%s listening for '%s' messages.\n", argv[0], topic[i]);
    }

    // for (i = 0; i < 9; i++)
    // {
    //     pthread_create(&global_thread[i], NULL, f[i], NULL);
    // }

    memset(&global_thread, 0, sizeof(global_thread));

    if((temp = pthread_create(&curr_thread, NULL, on_idle, NULL)) != 0)
        printf("faild to create 1st thread!\n"); 
    else 
        printf("created thread.\n");

    printf("LEDs : %d\n", numLEDs);
    printf("Press CTRL-D to exit.\n\n");
    
    /* block */
    while(fgetc(stdin) != EOF);
    
    /* disconnect */
    printf("\n%s disconnecting from %s\n", argv[0], addr);
    sleep(1);

    /* terminate */ 
    terminate(EXIT_SUCCESS, &client_daemon);
}

void terminate(int status, pthread_t *client_daemon)
{
    show(0);
    if (sockfd != -1) close(sockfd);
    if (fd) close(fd), fd = -1;
    if (pixels) free(pixels);
    if (client_daemon != NULL) pthread_cancel(*client_daemon);
    exit(status);
}

void publish_callback(void** unused, struct mqtt_response_publish *published) 
{
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char* topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    printf("[Received] %s \n", topic_name);

    switch(state){
        case 0:
            if (strcmp(topic_name, "hermes/hotword/toggleOff") == 0)
                last_state = state, state = 1;
            break;
        case 1:
            if (strcmp(topic_name, "hermes/asr/stopListening") == 0)
                last_state = state, state = 2;
            else if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                last_state = state, state = 0;
            break;
        case 2:
            if (strcmp(topic_name, "hermes/tts/say") == 0)
                last_state = state, state = 3;
            else if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                last_state = state, state = 0;
            break;
        case 3:
            if (strcmp(topic_name, "hermes/tts/sayFinished") == 0)
                last_state = state, state = 0;
            else if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                last_state = state, state = 0;
            break;
        
    }

    printf("[Info] State is changed to %d\n", state);
    fresh_state();
    
    free(topic_name);
}

void fresh_state(){
    if (last_state != state)
    {
        //void *thread_ret;
        //last_thread = curr_thread; 
        pthread_cancel(&curr_thread);
        //pthread_join(curr_thread,&thread_ret);
        //while(state_flag == 0);
        pthread_create(&curr_thread, NULL, f[state], NULL);

    }
}
void* client_refresher(void* client)
{
    while(1) 
    {
        mqtt_sync((struct mqtt_client*) client);
        usleep(100000U);
    }
    return NULL;
}

char* generate_client_id(){
    int i, flag;
    static char id[CLIENT_ID_LEN + 1] = {0};
    srand(time(NULL));
    for (int i = 0; i < CLIENT_ID_LEN; i++){
        flag = rand()%3;
        switch(flag){
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

void hw_init(){
    pixels = (uint8_t *)malloc(numLEDs * 4);
    begin();
}

void begin(){
    if((fd = open("/dev/spidev0.0", O_RDWR)) < 0) {
        printf("[Error] Can't open /dev/spidev0.0 (try 'sudo')");
        terminate(EXIT_FAILURE, NULL);
    }

    uint8_t mode = SPI_MODE_0 | SPI_NO_CS; //SPI_NO_CS   SPI_CS_HIGH

    ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &bitrate);
}

void set_color(uint32_t index, uint32_t color){
    uint8_t  r, g, b;

    r = color >> 16;
    g = color >>  8;
    b = color;

    if(index < numLEDs) {
        uint8_t *ptr = &pixels[index * 4];
        ptr[rOffset] = r;
        ptr[gOffset] = g;
        ptr[bOffset] = b;
    }
}

void show(uint8_t brightness){
    if (fd < 0) return;
    if (brightness > 255) brightness = 255;
    if (brightness < 0) brightness = 0;

    uint32_t    i;
    uint8_t     *ptr    =   pixels;
    uint16_t    scale   =   brightness;

    uint8_t x[4];
    // Start Signal:
    x[0] = 0;
    i    = 4;
    while(i--) (void)write(fd, x, 1);

    // Payload:
    x[0] = 0xFF;
    for(i=0; i<numLEDs; i++, ptr += 4) {

        x[1] = (ptr[1] * scale) >> 8;
        x[2] = (ptr[2] * scale) >> 8;
        x[3] = (ptr[3] * scale) >> 8;

        (void)write(fd, x, sizeof(x));
    }

    // Stop Signal:
    x[0] = 0;
    i = (numLEDs + 15) / 16;
    while(i--) (void)write(fd, x, 1);
}

void clear(){
    uint8_t *ptr;
    uint32_t i;
    for(ptr = pixels, i=0; i<numLEDs; i++, ptr += 4) {
        ptr[1] = 0x05; ptr[2] = 0x05; ptr[3] = 0x05;
    }
    show(5);
}

// States API

void *on_off(){
    clear();
    pthread_exit(1);
}

// 0
void *on_idle(){
    uint8_t i,j;
    int curr_bri = 0;
    printf("[Thread] ------>  on_idle started\n");
    state_flag = 0;
    clear();
    while(state == 0){
        // every 5 seconds
        for(i=0; i<numLEDs; i++){
            if(state != 0) {clear();state_flag = 1;return;}
            set_color(i, 0x00FF00);
        }
        while(curr_bri <= 64){
            if(state != 0) {clear();state_flag = 1;return;}
            show(curr_bri);
            curr_bri += 5;
            usleep(80000);
        }
        curr_bri = 64;
        while(curr_bri >= 0){
            if(state != 0) {clear();state_flag = 1;return;}
            show(curr_bri);
            curr_bri -= 5;
            usleep(80000);
        }
        curr_bri = 0;

        for (int j = 0; j < 100; j++)
        {
            // each 0.05s check
            if(state != 0) {clear();state_flag = 1;return;}
            usleep(50000);
        }

    }
    clear();
    state_flag = 1;
}

// 1
void *on_listen(){
    uint8_t i,g,group;
    printf("[Thread] ------>  on_listen started\n");
    state_flag = 0;
    clear();
    group = numLEDs/3;
    while(state == 1){
        for(i=0; i<3; i++){
            clear();
            for (g=0; g < group; g++)
                set_color(g*3+i, 0xFF0000);
            show(255);
            if(state != 1) {clear();state_flag = 1;return;}
            usleep(100000);
            show(5);
            if(state != 1) {clear();state_flag = 1;return;}
            usleep(100000);
            clear();
        }

        // every 2 seconds
        // show(5);
        // for(i=0; i<numLEDs; i++){
        //     if(state != 1) {clear();state_flag = 1;return;}
        //     set_color(i, 0xFF0000);
        // }
        // show(128);
        // for (int j = 0; j < 3; j++)
        // {
        //     // each 0.05s check
        //     if(state != 1) {clear();state_flag = 1;return;}
        //     usleep(50000);
        // }
        // show(5);
        // for (int j = 0; j < 3; j++)
        // {
        //     // each 0.05s check
        //     if(state != 1) {clear();state_flag = 1;return;}
        //     usleep(50000);
        // }

    }
    clear();
    state_flag = 1;     
}

// 2
void *on_think(){
    uint8_t i,g,group;
    printf("[Thread] ------>  on_think started\n");
    state_flag = 0;
    clear();
    group = numLEDs/3;
    while(state == 2){
        for(i=0; i<3; i++){
            clear();
            for (g=0; g < group; g++)
                set_color(g*3+i, 0x00FF00);
            show(255);
            if(state != 2) {clear();state_flag = 1;return;}
            usleep(100000);
            show(5);
            if(state != 2) {clear();state_flag = 1;return;}
            usleep(100000);
            clear();
        }
    }
    clear();
    state_flag = 1;
}

// 3
void *on_speak(){
    uint8_t i;
    printf("[Thread] ------>  on_speak started\n");
    state_flag = 0;
    while(1){
        if(state == 3){
            for(i=0; i<numLEDs; i++){
                clear();
                set_color(i, 0x00FF00);
                show(255);
                if(state != 3) {clear();state_flag = 1;return;}
                usleep(80000);
                show(0);
                if(state != 3) {clear();state_flag = 1;return;}
                usleep(80000);
                clear();
            }
        } 
    }
    clear();
    state_flag = 1;
}

// 4
void *to_mute(){
    uint8_t i;
    int curr_bri = 0;
    printf("[State] ------>  to_mute started\n");

    while(1){
        if (state == 4)
        {
            state = last_state;
            for(i=0; i<numLEDs; i++){
                if(state != 4) break;
                set_color(i, 0x00FFFF);
            }
            while(curr_bri <= 255){
                if(state != 4) break;
                show(curr_bri);
                curr_bri += 20;
                usleep(80000);
            }
            curr_bri = 255;
            while(curr_bri >= 0){
                if(state != 4) break;
                show(curr_bri);
                curr_bri -= 20;
                usleep(80000);
            }
        }
    }
}

// 5
void *to_unmute(){
    uint8_t i;
    int curr_bri = 0;
    printf("[State] ------>  to_unmute started\n");

    while(1){
        if (state == 5)
        {
            state = last_state;
            for(i=0; i<numLEDs; i++){
                if(state != 5) break;
                set_color(i, 0x00FF00);
            }
            while(curr_bri <= 255){
                if(state != 5) break;
                show(curr_bri);
                curr_bri += 20;
                usleep(80000);
            }
            curr_bri = 255;
            while(curr_bri >= 0){
                if(state != 5) break;
                show(curr_bri);
                curr_bri -= 20;
                usleep(80000);
            }
        }
    }
}

// 7 
void *on_success(){
    uint8_t i;
    int curr_bri = 0;
    printf("[State] ------>  on_success started\n");

    while(1){
        if (state == 7)
        {
            state = last_state;
            for(i=0; i<numLEDs; i++){
                if(state != 7) break;
                set_color(i, 0x00FF00);
            }
            while(curr_bri <= 255){
                if(state != 7) break;
                show(curr_bri);
                curr_bri += 20;
                usleep(20000);
            }
            curr_bri = 255;
            while(curr_bri >= 0){
                if(state != 7) break;
                show(curr_bri);
                curr_bri -= 20;
                usleep(20000);
            curr_bri = 0;  
            }
        }
    }
}

// 6
void *on_error(){
    uint8_t i;
    int curr_bri = 0;
    printf("[State] ------>  on_error started\n");

    while(1){
        if (state == 6)
        {
            state = last_state;
            for(i=0; i<numLEDs; i++){
                if(state != 6) break;
                set_color(i, 0x0000FF);
            }
            while(curr_bri <= 255){
                if(state != 6) break;
                show(curr_bri);
                curr_bri += 20;
                usleep(20000);
            }
            curr_bri = 255;
            while(curr_bri >= 0){
                if(state != 6) break;
                show(curr_bri);
                curr_bri -= 20;
                usleep(20000);
            curr_bri = 0;  
            }
        }
    }
}

// Led actions
void _all_breathe(uint8_t max_bri, unsigned long duration){
    int curr_bri = 0;

    while(curr_bri <= max_bri){
        show(curr_bri);
        curr_bri += 5;
        usleep(duration);
    }
    curr_bri = max_bri;
    while(curr_bri >= 0){
        show(curr_bri);
        curr_bri -= 5;
        usleep(duration);
    }
}



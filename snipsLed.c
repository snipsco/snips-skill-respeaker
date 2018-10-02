#include "include/snipsLed.h"

short       pre_state = 0;
short       state = 0;
char        client_id[CLIENT_ID_LEN + 1] = {0};

uint32_t    numLEDs;
uint32_t    bitrate = 32000000;
int         fd = -1;
int         sockfd = -1;
uint8_t     *pixels = NULL;
uint8_t     rOffset = 1;
uint8_t     gOffset = 2;
uint8_t     bOffset = 3;

pthread_t global_thread[9];
void (*f[9])(const char *)={on_idle, on_listen, on_think, on_speak, to_mute, to_unmute, on_success, on_error, on_off};

int main(int argc, const char *argv[]) 
{
    const char* addr;
    const char* port;
    const char* topic[NUM_TOPIC];
    int temp;
    generate_client_id();
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
        numLEDs = argv[3];
    } else {
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


    memset(&global_thread, 0, sizeof(global_thread));

    if((temp = pthread_create(&global_thread[state], NULL, on_idle, NULL)) != 0)
        printf("faild to create 1st thread!\n"); 
    else 
        printf("created thread.\n");

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
            if (strcmp(topic_name, "hermes/asr/startListening") == 0)
                pre_state = state, state = 1;
            else if (strcmp(topic_name, "hermes/feedback/sound/toggleOn") == 0)
                pre_state = state, state = 5;
            else if (strcmp(topic_name, "hermes/feedback/sound/toggleOff") == 0)
                pre_state = state, state = 4;
            else if (strcmp(topic_name, "hermes/feedback/led/toggleOff") == 0)
                pre_state = state, state = 9;
            else if (strcmp(topic_name, "hermes/tts/say") == 0)
                pre_state = state, state = 3;
            break;
        case 1:
            if (strcmp(topic_name, "hermes/asr/stopListening") == 0)
                pre_state = state, state = 2;
            break;
        case 2:
            if (strcmp(topic_name, "hermes/tts/say") == 0)
                pre_state = state, state = 3;
            else if (strcmp(topic_name, "hermes/nlu/intentParsed") == 0)
                pre_state = state, state = 6;
            else if (strcmp(topic_name, "hermes/nlu/intentNotRecognized") == 0)
                pre_state = state, state = 7;
            else if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                pre_state = state, state = 0;
            break;
        case 3:
            if (strcmp(topic_name, "hermes/tts/sayFinished") == 0)
                pre_state = state, state = 0;
            break;
        case 6:
            if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                pre_state = state, state = 0;
            else if (strcmp(topic_name, "hermes/tts/say") == 0)
                pre_state = state, state = 3;
            break;
        case 7:
            if (strcmp(topic_name, "hermes/tts/sayFinished") == 0)
                pre_state = state, state = 0;
            else if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                pre_state = state, state = 0;
            break;
        case 9:
            if (strcmp(topic_name, "hermes/tts/sayFinished") == 0)
                pre_state = state, state = 0;
            else if (strcmp(topic_name, "hermes/feedback/led/toggleOn") == 0)
                pre_state = state, state = 0;
            break;
        
    }
    fresh_state();
    printf("[Info] State is changed to %d\n", state);
    
    free(topic_name);
}

void fresh_state(){
    if (pre_state != state)
    {
        //pthread_cancel(&global_thread[pre_state]);
        pthread_create(&global_thread[state], NULL, f[state], NULL);
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

void generate_client_id(){
    
    int i, flag;

    srand(time(NULL));
    for (int i = 0; i < CLIENT_ID_LEN; i++)
    {
        flag = rand()%3;
        switch(flag){
            case 0:
                client_id[i] = rand()%26 + 'a';
                break;
            case 1:
                client_id[i] = rand()%26 + 'A';
                break;
            case 2:
                client_id[i] = rand()%10 + '0';
                break;
        }
    }
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
        ptr[1] = 0x00; ptr[2] = 0x00; ptr[3] = 0x00;
    }
}

// States API

void *on_off(){
    clear();
}

void *on_idle(){
    printf("[State] ------>  on_idle\n");
    uint8_t i;
    for(i=0; i<numLEDs; i++){
        set_color(i, 0x00FF00);
    }
    while(state == 0){
        for(i=0; i<numLEDs; i++){
            set_color(i, 0x00FF00);
        }
        _all_breathe(64, 80000);
        sleep(3);
    }
    //clear();
}

void *on_listen(){
    printf("[State] ------>  on_listen\n");
    uint8_t i;
    
    while(state == 1){
        for(i=0; i<numLEDs; i++){
            set_color(i, 0xFF0000);
        }
        _all_breathe(255, 2000);
        usleep(50000);
    }
    //clear();
}
void *on_think(){
    printf("[State] ------>  on_think\n");
    uint8_t i;
    while(state == 2){
        for(i=0; i<numLEDs; i++){
            set_color(i, 0xFF0000);
            show(125);
            usleep(80000);
            show(0);
            usleep(80000);
            clear();
        }
    }
}
void *on_speak(){
    printf("[State] ------>  on_speak\n");
    uint8_t i;
    while(state == 3){
        for(i=0; i<numLEDs; i++){
            set_color(i, 0x00FF00);
            show(125);
            usleep(80000);
            show(0);
            usleep(80000);
            clear();
        }
    }
}
void *to_mute(){
    printf("[State] ------>  to_mute\n");
    uint8_t i;
    for(i=0; i<numLEDs; i++){
        set_color(i, 0x00FFFF);
    }
    _all_breathe(64, 80000);
    pre_state = state, state = 0;
    fresh_state();
}
void *to_unmute(){
    printf("[State] ------>  to_unmute\n");
    uint8_t i;
    for(i=0; i<numLEDs; i++){
        set_color(i, 0xFFFF00);
    }
    _all_breathe(64, 80000);
    pre_state = state, state = 0;
    fresh_state();
}
void *on_success(){
    printf("[State] ------>  on_success\n");
    uint8_t i;
    for(i=0; i<numLEDs; i++){
        set_color(i, 0x00FF00);
    }
    while(state == 6)
        show(128);
    //clear();
}
void *on_error(){
    printf("[State] ------>  on_error\n");
    uint8_t i;
    for(i=0; i<numLEDs; i++){
        set_color(i, 0x0000FF);
    }
    
    while(state == 7) 
        show(128);
    //clear();
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



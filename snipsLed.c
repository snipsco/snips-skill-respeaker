#include "include/snipsLed.h"

short       pre_state = 0;
short       state = 0;
char        client_id[CLIENT_ID_LEN + 1] = {0};

uint32_t    numLEDs;
uint32_t    bitrate = 24000000;
int         fd = -1;
int         sockfd = -1;
uint8_t     *pixels = NULL;
uint8_t     rOffset = 1;
uint8_t     gOffset = 2;
uint8_t     bOffset = 3;

int main(int argc, const char *argv[]) 
{
    const char* addr;
    const char* port;
    const char* topic[NUM_TOPIC];
    
    generate_client_id();
    short i;
    printf("Client id is: %s\n", client_id);

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

    /* start publishing the time */
    
    printf("Press CTRL-D to terminate.\n\n");
    
    /* block */
    /* block */
    // fgetc(stdin) != EOF
    while(1){

        printf("Current state is %d\n", state);

        while(state == -1) on_off();
        while(state == 0) on_idle();
        while(state == 1) on_listen();
        while(state == 2) on_think();
        while(state == 3) on_speak();
        while(state == 4) to_mute(), state = 0;
        while(state == 5) to_unmute(), state = 0;
        while(state == 6) on_error();
        while(state == 7) on_success();

    } 
    
    /* disconnect */
    printf("\n%s disconnecting from %s\n", argv[0], addr);
    sleep(1);

    /* terminate */ 
    terminate(EXIT_SUCCESS, &client_daemon);
}

void terminate(int status, pthread_t *client_daemon)
{
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

    // hermes/hotword/detected
    // hermes/asr/toggle/on
    // hermes/asr/toggle/off
    // hermes/tts/say
    // hermes/tts/sayFinished
    // hermes/hotword/toggleOn

    // hermes/feedback/sound/toggleOn
    // hermes/feedback/sound/toggleOff

    printf("Received publish('%s'): %s\n", topic_name, (const char*) published->application_message);
    

    switch(state){
        case 0:
            if (strcmp(topic_name, "hermes/asr/startListening") == 0)
                state = 1;
            else if (strcmp(topic_name, "hermes/feedback/sound/toggleOn") == 0)
                state = 5;
            else if (strcmp(topic_name, "hermes/feedback/sound/toggleOff") == 0)
                state = 4;
            else if (strcmp(topic_name, "hermes/feedback/led/toggleOff") == 0)
                state = -1;
            else if (strcmp(topic_name, "hermes/tts/say") == 0)
                state = 3;
            break;
        case 1:
            if (strcmp(topic_name, "hermes/asr/stopListening") == 0)
                state = 2;
            break;
        case 2:
            if (strcmp(topic_name, "hermes/tts/say") == 0)
                state = 3;
            else if (strcmp(topic_name, "hermes/nlu/intentParsed") == 0)
                state = 6;
            else if (strcmp(topic_name, "hermes/nlu/intentNotRecognized") == 0)
                state = 7;
            break;
        case 3:
            if (strcmp(topic_name, "hermes/tts/sayFinished") == 0)
                state = 0;
            break;
        case 6:
            if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                state = 0;
            break;
        case 7:
            if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                state = 0;
            break;
        case -1:
            if (strcmp(topic_name, "hermes/feedback/led/toggleOn") == 0)
                state = 0;
            break;
    }   
    free(topic_name);
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
        printf("Can't open /dev/spidev0.0 (try 'sudo')");
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

void show(int brightness){
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

void on_off(){
    clear();
}

void on_idle(){
    _all_breathe(64, 50000, 0x00FF00);
}

void on_listen(){
    return;
}
void on_think(){
    return;
}
void on_speak(){
    return;
}
void to_mute(){
    return;
}
void to_unmute(){
    return; 
}
void on_error(){
    return;
}
void on_success(){
    return;
}

// Led actions

void _all_breathe(uint8_t max_bri, unsigned long duration, uint32_t color){
    int curr_bri = 0;
    uint32_t i;

    for(i=0; i<numLEDs; i++){
        set_color(i, color);
    }

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
    sleep(3);
}


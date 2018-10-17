#include "app.h"

/* global flags */
volatile sig_atomic_t   flag_terminate = 0;
short                   flag_update = 1,
                        flag_sleepmode = 0;


APA102      leds = {0, -1, NULL, 127};
short       curr_state = 0;

// self use
// devices number
int         fd_sock = -1;
pthread_t   curr_thread;
uint8_t     sleep_hour,
            sleep_minute,
            weak_hour,
            weak_minute;

char        rcv_site_id[255]= "";

const char	*addr;
const char	*port;

const char* topic[NUM_TOPIC]={
    "hermes/hotword/toggleOff",
    "hermes/asr/startListening",
    "hermes/asr/stopListening",
    "hermes/tts/say",
    "hermes/tts/sayFinished",
    "hermes/hotword/toggleOn",
    "hermes/feedback/sound/toggleOn",
    "hermes/feedback/sound/toggleOff",
    "hermes/nlu/intentNotRecognized",
    "hermes/nlu/intentParsed",
    "hermes/feedback/led/toggleOn",
    "hermes/feedback/led/toggleOff",
};

snipsSkillConfig configList[]={
    {"model", 0},       //0
    {"spi_dev", 0},     //1
    {"led_num", 0},     //2
    {"led_bri", 0},     //3
    {"mqtt_host", 0},   //4
    {"mqtt_port", 0},   //5
    {"on_idle", 0},     //6
    {"on_listen", 0},   //7
    {"on_speak", 0},    //8
    {"to_mute", 0},     //9
    {"to_unmute", 0},   //10
    {"on_success", 0},  //11
    {"on_error", 0},    //12
    {"nightmode", 0},   //13
    {"go_sleep", 0},    //14
    {"go_weak", 0},     //15
    {"on_off", "1"},    //16
    {"site_id", 0}      //17
};

const char* status_s[]={
    "on_idle",      //0
    "on_listen",    //1
    "on_think",     //2
    "on_speak",     //3
    "to_mute",      //4
    "to_unmute",    //5
    "on_success",   //6
    "on_error",     //7
    "on_off"        //8
};

void (*status[9])(const char *)={ 
    on_idle, 
    on_listen, 
    on_think, 
    on_speak, 
    to_mute, 
    to_unmute, 
    on_success, 
    on_error, 
    on_off
};

int main(int argc, char const *argv[])
{	
    int i;
    char *client_id;

    // generate a random id as client id
    client_id = generate_client_id();
    signal(SIGINT, int_handler);
    // get config.ini
    config(configList, CONFIG_NUM);

    switch_on_power();
    // get input parameters
    leds.numLEDs = (argc > 1)? atoi(argv[1]) : atoi(configList[2].value);
    addr = (argc > 2)? argv[2] : configList[4].value; // mqtt_host
    port = (argc > 3)? argv[3] : configList[5].value; // mqtt_port

    // get brightness
    leds.brightness = (strlen(configList[3].value) != 0) ? atoi(configList[3].value) : 127;

    // if sleep mode is enabled
    if (if_config_true("nightmode", configList, NULL) == 1){
        flag_sleepmode = 1;
        parse_hour_minute(configList[14].value, &sleep_hour, &sleep_minute);
        parse_hour_minute(configList[15].value, &weak_hour, &weak_minute);
    }
    
    /* open the non-blocking TCP socket (connecting to the broker) */
    int sockfd = open_nb_socket(addr, port);
    if (sockfd == -1) {
        perror("Failed to open socket: ");
        close_all(EXIT_FAILURE, NULL);
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
        close_all(EXIT_FAILURE, NULL);
    }
    /* start a thread to refresh the client (handle egress and ingree client traffic) */
    pthread_t client_daemon;
    if(pthread_create(&client_daemon, NULL, client_refresher, &client)) {
        fprintf(stderr, "Failed to start client daemon.\n");
        close_all(EXIT_FAILURE, NULL);
    }
    /* subscribe */
    for(i=0;i<NUM_TOPIC;i++){
        mqtt_subscribe(&client, topic[i], 0);
        printf("[Info] Subscribed to '%s'.\n", topic[i]);
    }
    for(i=0;i<CONFIG_NUM;i++){
        printf("[Conf] %s - '%s'\n", configList[i].key, configList[i].value);
    }
    apa102_spi_setup();
    /* start publishing the time */
    printf("[Info] Initilisation looks good.....\n");
    printf("[Info] Client id : %s\n", client_id);
    printf("[Info] Program : %s\n", argv[0]);
    printf("[Info] LED number : %d with max brightness: %d\n", leds.numLEDs, leds.brightness);
    printf("[Info] Device : %s\n", configList[0].value);
    printf("[Info] Listening to MQTT bus: %s:%s \n",addr, port);
    printf("[Info] Press CTRL-C to exit.\n\n");

    /* block */
    while(1){
        if(flag_sleepmode)
            check_nightmode();

        if (flag_update) 
            update_state_machine();

        if (flag_terminate) break;

        usleep(10000);
    }
    // disconnect
    printf("[Info] %s disconnecting from %s\n", argv[0], addr);
    sleep(1);

    // clean
    close_all(EXIT_SUCCESS, &client_daemon);
    return 0;
}

void update_state_machine(void){
    void *ret_val="NONE";

    if (if_config_true(status_s[curr_state], configList, NULL) == 1){
        printf("[Debug] State is changed to %d\n", curr_state);
        // block until the previous terminate
        pthread_join(curr_thread,&ret_val);
        printf("[Debug] Previous thread %s terminated with success\n",(char*)ret_val);
        pthread_create(&curr_thread, NULL, status[curr_state], NULL);
    }else{
        flag_update = 0;
    }
}

void check_nightmode(void){
    time_t curr_time;
    struct tm *read_time = NULL;

    curr_time = time(NULL);
    read_time = localtime(&curr_time);

    if(read_time->tm_hour == sleep_hour && 
        read_time->tm_min == sleep_minute &&
        curr_state != 8){
        curr_state = 8;
        flag_update = 1;
        printf("[Info] ------>  Nightmode started\n");
    }
    if(read_time->tm_hour == weak_hour && 
        read_time->tm_min == weak_minute &&
        curr_state == 8){
        curr_state = 0;
        flag_update = 1;
        printf("[Info] ------>  Nightmode terminated\n");
    }
}

void switch_on_power(void){
    int fd_gpio;
    char gpio_66[] = {'6','6'};
    char direction[] = {'o','u','t'};
    char value[] = {'0'};

    if(if_config_true("model", configList, "rsp_corev2")){
        // export gpio
        if(fd_gpio = open("/sys/class/gpio/export", O_RDWR)<0){
            printf("[Error] Can't open gpio export, please manually echo '66' to this file\n");
            return;
        }

        if(write(fd_gpio, gpio_66, sizeof(gpio_66))>0){
            printf("[Info] Exported GPIO66..\n");
            close(fd_gpio);
        }
        else{
            close(fd_gpio);
            printf("[Info] Failed to GPIO66..\n");
            return;
        }
        // set direction
        if(fd_gpio = open("/sys/class/gpio/gpio66/direction", O_RDWR)<0){
            printf("[Error] Can't open gpio export/gpio66/direction, please manually echo 'out' to this file\n");
            return;
        }
        
        if(write(fd_gpio, direction, sizeof(direction))>0){
            printf("[Info] Set GPIO66 direction out..\n");
            close(fd_gpio);
        }
        else{
            close(fd_gpio);
            printf("[Info] Failed to set GPIO66 direction， please manually echo 'out' to this file\n");
            return;
        }
        // enable by set to 0
        if(fd_gpio = open("/sys/class/gpio/gpio66/value", O_RDWR)<0){
            printf("[Error] Can't open gpio export/gpio66/value, please manually echo '0' to this file\n");
            return;
        }
        
        if(write(fd_gpio, value, sizeof(value))>0){
            printf("[Info] Set GPIO66 value 0..\n");
            close(fd_gpio);
        }
        else{
            close(fd_gpio);
            printf("[Info] Failed to set GPIO66 value 0, please manually echo '0' to this file\n");
            return;
        }
    }
}

void apa102_spi_setup(void){
    int temp,i;
    leds.pixels = (uint8_t *)malloc(leds.numLEDs * 4);
    if (begin()){
        for (i = 0; i < 3; i++){
            printf("[Error] Failed to start SPI! Retrying..%d\n",i+1); 
            sleep(30);
            if (begin() == 0) return;
        }
        printf("[Error] Failed to start SPI!\n"); 
        close_all(EXIT_FAILURE, NULL);
    }
}

void publish_callback(void** unused, struct mqtt_response_publish *published) {
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char *topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';
    printf("[Received] %s \n", topic_name);
    
    get_site_id(published->application_message);

    if (strcmp(configList[17].value, rcv_site_id) != 0)
        return;

    printf("[Site ID] %s\n", rcv_site_id);
    
    switch(curr_state){
        case 0: // on idle
            if (strcmp(topic_name, "hermes/hotword/toggleOff") == 0)
                flag_update = 1,curr_state = 1;
            else if (strcmp(topic_name, "hermes/feedback/sound/toggleOff") == 0)
                flag_update = 1,curr_state = 4;
            else if (strcmp(topic_name, "hermes/feedback/sound/toggleOn") == 0)
                flag_update = 1,curr_state = 5;
            else if (strcmp(topic_name, "hermes/tts/say") == 0)
                flag_update = 1,curr_state = 3;
            else if (strcmp(topic_name, "hermes/feedback/led/toggleOff") == 0)
                flag_update = 1,curr_state = 8;
            break;
        case 1: // on listen
            if (strcmp(topic_name, "hermes/nlu/intentParsed") == 0)
                flag_update = 1,curr_state = 6;
            else if (strcmp(topic_name, "hermes/nlu/intentNotRecognized") == 0)
                flag_update = 1,curr_state = 7;
            else if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                flag_update = 1,curr_state = 0;
            break;
        case 2: // on think -> too fast to perform
            if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                flag_update = 1,curr_state = 0;
            break;
        case 3: // on speak
            if (strcmp(topic_name, "hermes/tts/sayFinished") == 0)
                flag_update = 1,curr_state = 0;
            else if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                flag_update = 1,curr_state = 0;
            break;
        case 4: // to mute
            if (strcmp(topic_name, "hermes/hotword/toggleOff") == 0)
                flag_update = 1,curr_state = 1;
            break;
        case 5:// to unmute
            if (strcmp(topic_name, "hermes/hotword/toggleOff") == 0)
                flag_update = 1,curr_state = 1;
            break;
        case 6: // on success
            if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                flag_update = 1,curr_state = 0;
            break;
        case 7: // on error
            if (strcmp(topic_name, "hermes/hotword/toggleOn") == 0)
                flag_update = 1,curr_state = 0;
            break;
        case 8: // on off
            if (strcmp(topic_name, "hermes/feedback/led/toggleOn") == 0)
                flag_update = 1,curr_state = 0;
            break;
    }

    free(topic_name);
}

void* client_refresher(void* client){
    while(1) 
    {
        mqtt_sync((struct mqtt_client*) client);
        usleep(100000U);
    }
    return NULL;
}

char *generate_client_id(void){
    int i ,seed;
    static char id[CLIENT_ID_LEN + 1] = {0};
    srand(time(NULL));
    for (i = 0; i < CLIENT_ID_LEN; i++){
        seed = rand()%3;
        switch(seed){
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

void close_all(int status, pthread_t *client_daemon){
    int fd_gpio;
    char gpio_66[]={'6','6'};

    clear();
    if(if_config_true("model", configList, "rsp_corev2")){
        fd_gpio = open("/sys/class/gpio/unexport", O_RDWR);
        if(write(fd_gpio, gpio_66, sizeof(gpio_66))){
            printf("[Info] Closed GPIO66..\n");
            close(fd_gpio);
        }  
    }
    if (fd_sock != -1) close(fd_sock);
    if (leds.fd_spi != -1) close(leds.fd_spi);
    if (leds.pixels) free(leds.pixels);
    if (client_daemon != NULL) pthread_cancel(*client_daemon);
    pthread_cancel(curr_thread);
    exit(status);
}

static void get_site_id(char *msg){
    char *start;
    int count = 0;
    start = strstr(msg, "\"siteId\":\""); // len = 10
    if (start == NULL )
        return;
    start += 10;
    while(*start != '\"'){
        rcv_site_id[count] = *start;
        start += 1;
        count += 1;
    }
    rcv_site_id[count] = '\0';
}

static void int_handler(int sig){
    flag_terminate = 1;
}


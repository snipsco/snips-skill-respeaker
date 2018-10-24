#include "main.h"
#include "apa102.h"
#include "get_config.h"
#include "state_handler.h"
#include "rsp_corev2.h"

#include <pthread.h>
#include <signal.h>
#include <posix_sockets.h>

static void get_site_id(const char *msg);
static void interrupt_handler(int sig);

volatile sig_atomic_t   flag_terminate = 0;
short                   flag_update = 1;
short                   flag_sleepmode = 0;

APA102      leds = {0, -1, NULL, 127};
STATE       curr_state = ON_IDLE;

int         fd_sock = -1;
pthread_t   curr_thread;
uint8_t     sleep_hour;
uint8_t     sleep_minute;
uint8_t     weak_hour;
uint8_t     weak_minute;

char        rcv_site_id[255]= "";

const char	*addr;
const char	*port;

const char* topics[]={
    HOT_OFF,
    STA_LIS,
    END_LIS,
    STA_SAY,
    END_SAY,
    HOT_ON,
    SUD_ON,
    SUD_OFF,
    LED_ON,
    LED_OFF
};

snipsSkillConfig configList[CONFIG_NUM]=
{
    {{C_MODEL_STR}, {0}},
    {{C_SPI_DEV_STR}, {0}},
    {{C_LED_NUM_STR}, {0}},
    {{C_LED_BRI_STR}, {0}},
    {{C_MQTT_HOST_STR}, {0}},
    {{C_MQTT_PORT_STR}, {0}},
    {{C_ON_IDLE_STR}, {0}},
    {{C_ON_LISTEN_STR}, {0}},
    {{C_ON_SPEAK_STR}, {0}},
    {{C_TO_MUTE_STR}, {0}},
    {{C_TO_UNMUTE_STR}, {0}},
    {{C_NIGHTMODE_STR}, {0}},
    {{C_GO_SLEEP_STR}, {0}},
    {{C_GO_WEAK_STR}, {0}},
    {{C_ON_DISABLED_STR}, {"1"}},
    {{C_SITE_ID_STR}, {0}}
};

int main(int argc, char const *argv[])
{	
    int i;
    char *client_id;
    // generate a random id as client id
    client_id = generate_client_id();
    signal(SIGINT, interrupt_handler);
    // get config.ini
    read_config_file(configList, CONFIG_NUM);

    switch_on_power();
    // get input parameters
    leds.numLEDs = (argc > 1)? atoi(argv[1]) : atoi(configList[C_LED_NUM].value);
    addr = (argc > 2)? argv[2] : configList[C_MQTT_HOST].value; // mqtt_host
    port = (argc > 3)? argv[3] : configList[C_MQTT_PORT].value; // mqtt_port

    // get brightness
    leds.brightness = (strlen(configList[C_LED_BRI].value) != 0) ? atoi(configList[C_LED_BRI].value) : 127;

    // if sleep mode is enabled
    if (if_config_true("nightmode", configList, NULL) == 1){
        flag_sleepmode = 1;
        parse_hour_minute(configList[C_GO_SLEEP].value, &sleep_hour, &sleep_minute);
        parse_hour_minute(configList[C_GO_WEAK].value, &weak_hour, &weak_minute);
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
        fprintf(stderr, "[Error] %s\n", mqtt_error_str(client.error));
        close_all(EXIT_FAILURE, NULL);
    }
    /* start a thread to refresh the client (handle egress and ingree client traffic) */
    pthread_t client_daemon;
    if(pthread_create(&client_daemon, NULL, client_refresher, &client)) {
        fprintf(stderr, "[Error] Failed to start client daemon.\n");
        close_all(EXIT_FAILURE, NULL);
    }
    /* subscribe */
    for(i=0;i<NUM_TOPIC;i++){
        mqtt_subscribe(&client, topics[i], 0);
        fprintf(stdout, "[Info] Subscribed to '%s'.\n", topics[i]);
    }
    for(i=0;i<CONFIG_NUM;i++){
        fprintf(stdout, "[Conf] %s - <%s>\n", configList[i].key, configList[i].value);
    }

    if(!apa102_spi_setup())
        close_all(EXIT_FAILURE, NULL);

    /* start publishing the time */
    fprintf(stdout, "[Info] Initilisation looks good.....\n");
    fprintf(stdout, "[Info] Client id : %s\n", client_id);
    fprintf(stdout, "[Info] Program : %s\n", argv[0]);
    fprintf(stdout, "[Info] LED number : %d with max brightness: %d\n", leds.numLEDs, leds.brightness);
    fprintf(stdout, "[Info] Device : %s\n", configList[C_MODEL].value);
    fprintf(stdout, "[Info] Listening to MQTT bus: %s:%s \n",addr, port);
    fprintf(stdout, "[Info] Press CTRL-C to exit.\n\n");

    /* block */
    while(1){
        if(flag_sleepmode)
            check_nightmode();

        if (flag_update) 
            state_machine_update();

        if (flag_terminate) break;

        usleep(10000);
    }
    // disconnect
    fprintf(stdout, "[Info] %s disconnecting from %s\n", argv[0], addr);
    sleep(1);

    // clean
    close_all(EXIT_SUCCESS, &client_daemon);
    return 0;
}

void check_nightmode(void){
    time_t curr_time;
    struct tm *read_time = NULL;

    curr_time = time(NULL);
    read_time = localtime(&curr_time);

    if(read_time->tm_hour == sleep_hour && 
        read_time->tm_min == sleep_minute &&
        curr_state != ON_DISABLED){
        curr_state = ON_DISABLED;
        flag_update = 1;
        fprintf(stdout, "[Info] ------>  Nightmode started\n");
    }
    if(read_time->tm_hour == weak_hour && 
        read_time->tm_min == weak_minute &&
        curr_state == ON_DISABLED){
        curr_state = ON_IDLE;
        flag_update = 1;
        fprintf(stdout, "[Info] ------>  Nightmode terminated\n");
    }
}

void publish_callback(void** unused, struct mqtt_response_publish *published) {
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char *topic_name = (char*) malloc(published->topic_name_size + 1);

    memcpy(topic_name, published->topic_name, published->topic_name_size);

    topic_name[published->topic_name_size] = '\0';
    get_site_id(published->application_message);
    if (strcmp(configList[C_SITE_ID].value, rcv_site_id) != 0)
        return;

    fprintf(stdout, "[Received] %s on Site: %s\n", topic_name, rcv_site_id);

    state_handler_main(topic_name);

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
            fprintf(stdout, "[Info] Closed GPIO66..\n");
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

static void get_site_id(const char *msg){
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

static void interrupt_handler(int sig){
    flag_terminate = 1;
}


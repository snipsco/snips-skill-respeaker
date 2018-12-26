#include "mqtt_client.h"
#include "state_handler.h"

#include <mqtt.h>
#include <posix_sockets.h>
#include <pthread.h>

static void mqtt_callback_handler(void** unused, struct mqtt_response_publish *published);
static void get_site_id(const char *msg);
static void* mqtt_client_refresher(void* mqtt_client);

struct      mqtt_client mqtt_client;
pthread_t   mqtt_client_daemon;
int         fd_mqtt_sock = -1;
char        rcv_site_id[255]= "";
uint8_t     mqtt_sendbuf[2048];
uint8_t     mqtt_recvbuf[1024];

const char *topics[]={
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

uint8_t start_mqtt_client(const char *mqtt_client_id,
                          const char *mqtt_addr,
                          const char *mqtt_port,
                          const char *username,
                          const char *password){

    fd_mqtt_sock = open_nb_socket(mqtt_addr, mqtt_port);
    if (fd_mqtt_sock == -1) {
        perror("[Error] Failed to open socket: ");
        return 0;
    }

    mqtt_init(&mqtt_client,
              fd_mqtt_sock,
              mqtt_sendbuf,
              sizeof(mqtt_sendbuf),
              mqtt_recvbuf,
              sizeof(mqtt_recvbuf),
              mqtt_callback_handler);

    mqtt_connect(&mqtt_client,
                 mqtt_client_id,
                 NULL,
                 NULL,
                 0,
                 username,
                 password,
                 0,
                 400);

    if (mqtt_client.error != MQTT_OK) {
        fprintf(stderr, "error: %s\n", mqtt_error_str(mqtt_client.error));
        return 0;
    }

    if(pthread_create(&mqtt_client_daemon, NULL, mqtt_client_refresher, &mqtt_client)) {
        fprintf(stderr, "[Error] Failed to start client daemon.\n");
        return 0;
    }

    for(int i=0;i<NUM_TOPIC;i++)
        if( MQTT_OK != mqtt_subscribe(&mqtt_client, topics[i], 0)){
            printf("[Error] Subscribe error\n");
            return 0;
        }
    return 1;
}

void terminate_mqtt_client(void){
    fprintf(stdout, "[Info] disconnecting mqtt \n");
    sleep(1);
    if (fd_mqtt_sock != -1)
        close(fd_mqtt_sock);
    if (mqtt_client_daemon)
        pthread_cancel(mqtt_client_daemon);
}

static void mqtt_callback_handler(void** unused, struct mqtt_response_publish *published){
    printf("[Debug] new handler is called\n");

    char *topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    get_site_id(published->application_message);
    if (strcmp("default", rcv_site_id) != 0)
        return;

    fprintf(stdout, "[Received] %s on Site: %s\n", topic_name, rcv_site_id);

    state_handler_main(topic_name);

    free(topic_name);
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

static void* mqtt_client_refresher(void* mqtt_client){
    while(1){
        mqtt_sync((struct mqtt_client*) mqtt_client);
        usleep(100000U);
    }
    return NULL;
}
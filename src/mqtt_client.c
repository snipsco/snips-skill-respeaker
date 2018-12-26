#include "mqtt_client.h"
#include "state_handler.h"

#include <mqtt.h>
#include <posix_sockets.h>
#include <pthread.h>

void publish_callback(void** unused, struct mqtt_response_publish *published);
static void get_site_id(const char *msg);

pthread_t   client_daemon;
int         fd_sock = -1;

char        rcv_site_id[255]= "";

struct mqtt_client client;
uint8_t sendbuf[2048];
uint8_t recvbuf[1024];

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

/**
 * Description: create and start a mqtt client
 * Input:
 *    client_id
 *    addr
 *    port
 *    username
 *    password
 * Return:
 *    0: failded
 *    1: successful
 */

uint8_t start_mqtt_client(const char	*client_id, const char	*addr, const char	*port, const char  *username, const char  *password){
    /* open the non-blocking TCP socket (connecting to the broker) */
    fd_sock = open_nb_socket(addr, port);
    if (fd_sock == -1) {
        perror("[Error] Failed to open socket: ");
        return 0;
    }
    /* setup a client */

    mqtt_init(&client, fd_sock, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback);

    if (*username && *password)
        mqtt_connect(&client, client_id, NULL, NULL, 0, username, password, 0, 400);
    else
        mqtt_connect(&client, client_id, NULL, NULL, 0, NULL, NULL, 0, 400);

    /* check for errors */
    if (client.error != MQTT_OK) {
        fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
        return 0;
    }

    /* start a thread to refresh the client (handle egress and ingree client traffic) */
    if(pthread_create(&client_daemon, NULL, client_refresher, &client)) {
        fprintf(stderr, "[Error] Failed to start client daemon.\n");
        return 0;
    }


    /* subscribe */
    for(int i=0;i<NUM_TOPIC;i++)
        if( MQTT_OK != mqtt_subscribe(&client, topics[i], 0)){
            printf("[Error] Subscribe error\n");
            return 0;
        }
    return 1;
}

void publish_callback(void** unused, struct mqtt_response_publish *published) {
    printf("callback is called \n");
    /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
    char *topic_name = (char*) malloc(published->topic_name_size + 1);

    memcpy(topic_name, published->topic_name, published->topic_name_size);

    printf("received something: %s\n", (const char*)published->application_message);
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

void* client_refresher(void* client){
    printf("client refresher started\n");
    while(1){
        mqtt_sync((struct mqtt_client*) client);
        usleep(100000U);
    }
    return NULL;
}

void terminate_mqtt_client(){
    // disconnect
    fprintf(stdout, "[Info] disconnecting mqtt \n");
    sleep(1);
    if (fd_sock != -1)
        close(fd_sock);
    if (client_daemon)
        pthread_cancel(client_daemon);
}
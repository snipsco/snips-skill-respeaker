#include "mqtt_client.h"
#include "state_handler.h"
#include "cJSON.h"
#include "mqtt.h"
#include "posix_sockets.h"
#include "verbose.h"

#include <pthread.h>

static void mqtt_callback_handler(void** unused, struct mqtt_response_publish *published);
static int match_site_id(const char *message);
static void* mqtt_client_refresher(void* mqtt_client);

extern SNIPS_RUN_PARA RUN_PARA;

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

int start_mqtt_client(const char *mqtt_client_id,
                      const char *mqtt_addr,
                      const char *mqtt_port,
                      const char *username,
                      const char *password){

    fd_mqtt_sock = open_nb_socket(mqtt_addr, mqtt_port);
    if (fd_mqtt_sock == -1) {
        perror("[Error] Failed to open socket: ");
        return -1;
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
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" %s", __FUNCTION__, mqtt_error_str(mqtt_client.error));
        return -1;
    }

    if(pthread_create(&mqtt_client_daemon, NULL, mqtt_client_refresher, &mqtt_client)) {
        verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Failed to start client daemon", __FUNCTION__);
        return -1;
    }

    for(int i=0;i<NUM_TOPIC;i++)
        if( MQTT_OK != mqtt_subscribe(&mqtt_client, topics[i], 0)){
            verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" Subscribe error", __FUNCTION__);
            return -1;
        }
    return 0;
}

void terminate_mqtt_client(void){
    verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Disconnecting mqtt", __FUNCTION__);
    sleep(1);
    if (fd_mqtt_sock != -1)
        close(fd_mqtt_sock);
    if (mqtt_client_daemon)
        pthread_cancel(mqtt_client_daemon);
}

void mqtt_hotword_trigger(void){
    char *topic = "hermes/dialogueManager/startSession";
    char application_message[1024];
    sprintf(application_message, "{\"siteId\":\"%s\",\"init\":{\"type\":\"action\"}}", RUN_PARA.snips_site_id);
    mqtt_publish(&mqtt_client, topic, application_message, strlen(application_message), 0);
    verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" Publishing to topic : %s ", __FUNCTION__, topic);
    verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" Publishing message : %s ", __FUNCTION__, application_message);
}

void mqtt_mute_feedback(void){
    char *topic = "hermes/feedback/sound/toggleOff";
    char application_message[1024];
    sprintf(application_message, "{\"siteId\":\"%s\"}", RUN_PARA.snips_site_id);
    mqtt_publish(&mqtt_client, topic, application_message, strlen(application_message), 0);
    verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Muting the feedback sound", __FUNCTION__);
    verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" Publishing to topic : %s ", __FUNCTION__, topic);
    verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" Publishing message : %s ", __FUNCTION__, application_message);
}

void mqtt_unmute_feedback(void){
    char *topic = "hermes/feedback/sound/toggleOn";
    char application_message[1024];
    sprintf(application_message, "{\"siteId\":\"%s\"}", RUN_PARA.snips_site_id);
    mqtt_publish(&mqtt_client, topic, application_message, strlen(application_message), 0);
    verbose(VV_INFO, stdout, BLUE"[%s]"NONE" Unmuting the feedback sound", __FUNCTION__);
    verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" Publishing to topic : %s ", __FUNCTION__, topic);
    verbose(VVV_DEBUG, stdout, BLUE"[%s]"NONE" Publishing message : %s ", __FUNCTION__, application_message);
}

static void mqtt_callback_handler(void** unused, struct mqtt_response_publish *published){
    char *topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    verbose(VVV_DEBUG, stdout, "[Receive] topic "PURPLE" %s "NONE, topic_name);
    //if(strcmp(topic_name, END_SAY))
    if (!match_site_id(published->application_message))
        return;
    state_handler_main(topic_name);
    free(topic_name);
}

/**
 * @brief: check if the coming message corresponds to specified siteId
 *
 * @param[in] mqtt_received_message
 * @param[in] target_site_id
 *
 * @returns \0 not match or \1 match \-1 error
 */
static int match_site_id(const char *message){
    const cJSON *rev_site_id = NULL;

    cJSON *payload_json = cJSON_Parse(message);
    if (payload_json == NULL){
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
            verbose(V_NORMAL, stderr, BLUE"[%s]"NONE" from parsing message : %s", __FUNCTION__, error_ptr);
        return -1;
    }

    rev_site_id = cJSON_GetObjectItemCaseSensitive(payload_json, "siteId");
    if (rev_site_id == NULL)
        return 1;

    if(!strcmp(RUN_PARA.snips_site_id, rev_site_id->valuestring)){
        verbose(VVV_DEBUG, stdout, "Received from site" GREEN " %s "NONE, RUN_PARA.snips_site_id, rev_site_id->valuestring);
        cJSON_Delete(payload_json);
        return 1;
    }
    else{
        verbose(VV_INFO, stdout, "Received from site" RED " %s "NONE, RUN_PARA.snips_site_id, rev_site_id->valuestring);
        cJSON_Delete(payload_json);
        return 0;
    }
}

static void* mqtt_client_refresher(void* mqtt_client){
    while(1){
        mqtt_sync((struct mqtt_client*) mqtt_client);
        usleep(100000U);
    }
    return NULL;
}
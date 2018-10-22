#ifndef __SNIPS_LED_H__
	#include <snips_led.h>
#endif

#ifndef __TYPEDEF_H__
	#include <typedef.h>
#endif

#ifndef __MQTT_H__
	#include <mqtt.h>
#endif

#define CLIENT_ID_LEN		10
#define NUM_TOPIC			12

void update_state_machine(void);
void check_nightmode(void);
void apa102_spi_setup(void);
void publish_callback(void** unused, struct mqtt_response_publish *published);
void *client_refresher(void* client);
char *generate_client_id(void);
void close_all(int status, pthread_t *client_daemon);
static void get_site_id(char *msg);
static void int_handler(int sig);
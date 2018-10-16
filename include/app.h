#include <snips_led.h>

#ifndef __TYPEDEF__H__
	#include <typedef.h>
#endif

#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#include <mqtt.h>
#include <posix_sockets.h>

#include <apa102.h>
#include <get_config.h>
#include <animation.h>



#define CLIENT_ID_LEN 	10
#define NUM_TOPIC 		12

void update_state_machine(void);
void check_nightmode(void);
void apa102_spi_setup(void);
void publish_callback(void** unused, struct mqtt_response_publish *published);
void *client_refresher(void* client);
char *generate_client_id(void);
void close_all(int status, pthread_t *client_daemon);
static void int_handler(int sig);

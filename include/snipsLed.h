#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#include <mqtt.h>
#include "posix_sockets.h"

#define CLIENT_ID_LEN 10
#define NUM_TOPIC 10

// MQTT
void generate_client_id();
void publish_callback(void** unused, struct mqtt_response_publish *published);
void*client_refresher(void* client);
void terminate(int status, pthread_t *client_daemon);

// LED
void hw_init();
void begin();
void set_color(uint32_t index, uint32_t vcolor);
void show(int brightness);
void clear();

// States
void on_off();
void on_idle();
void on_listen();
void on_think();
void on_speak();
void to_mute();
void to_unmute();
void on_error();
void on_success();

// Basic actions
void _all_breathe(uint8_t max_bri, unsigned long duration, uint32_t color);

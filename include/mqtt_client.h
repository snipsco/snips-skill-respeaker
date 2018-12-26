#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include <common.h>

uint8_t start_mqtt_client(
                          const char	*client_id,
                          const char	*addr,
                          const char	*port,
                          const char  *username,
                          const char  *password);
void* client_refresher(void* client);
void terminate_mqtt_client(void);
#endif
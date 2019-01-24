#ifndef __MQTT_CLIENT_H__
#define __MQTT_CLIENT_H__

#include "common.h"

/**
 * @brief: create and start a mqtt client
 *
 * @param[in] mqtt_client_id
 * @param[in] mqtt_addr
 * @param[in] mqtt_port
 * @param[in] username
 * @param[in] password
 *
 * @returns 0 failded or 1 successful
 */
int start_mqtt_client(const char *mqtt_client_id,
                      const char *mqtt_addr,
                      const char *mqtt_port,
                      const char *username,
                      const char *password);

/**
 * @brief: create and start a mqtt client
 *
 * @param[in] mqtt_client_id
 * @param[in] mqtt_addr
 * @param[in] mqtt_port
 * @param[in] username
 * @param[in] password
 *
 * @returns 0 failded or 1 successful
 */
void terminate_mqtt_client(void);

void mqtt_hotword_trigger(void);
void mqtt_mute_feedback(void);
void mqtt_unmute_feedback(void);

#endif
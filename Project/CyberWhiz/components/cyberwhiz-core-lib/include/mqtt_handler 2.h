/**
 * @file mqtt_handler.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2024-11-11
 * 
 * @copyright, Copyright (C) 2024 by CyberWhiz
 * 
 *    All Rights Reserved.
 *    Permission to use, reproduce, copy, prepare derivative works,
 *    modify, distribute, perform, display or sell this software and/or
 *    its documentation for any purpose is prohibited without the express
 *    written consent of CyberWhiz
 * 
 */

#ifndef MQTT_HANDLER_H_
#define MQTT_HANDLER_H_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#define MQTT_CONNECT_TIMEOUT_IN_SECS 45
#define MAX_MQTT_MESSAGE_SIZE 2048

extern char mqtt_rx_buffer[MAX_MQTT_MESSAGE_SIZE];
extern char otaURL[1536];
extern int mqtt_rx_offset;

#define SHADOW_UPDATE_PERIOD 5
#define NAMED_SHADOW_UPDATE_PERIOD 10

typedef struct {
    char telemetry_topic[128];
    char rpc_topic[128];
    char shadow_topic[128];
    char shadow_systemInfo_topic[128];
    char shadow_config_topic[128];
    char shadow_maintenance_topic[128];
    char shadow_errors_topic[128];
    char shadow_modes_topic[128];
    char shadow_settings_topic[128];
} topic_set_t;

extern topic_set_t topics;

void mqtt_connect(void);
void mqtt_disconnect(void);
int mqtt_publish(const char *topic, const char *data);
int mqtt_subscribe(const char *topic);
void generateTopics(int boardType, const uint8_t *ble_mac, topic_set_t *topics);
void shadowUpdateCallback(TimerHandle_t xTimer);
void namedShadowUpdateCallback(TimerHandle_t xTimer);
void durationMQTTTimerCallback(TimerHandle_t xTimer);

#endif /* MQTT_HANDLER_H_ */

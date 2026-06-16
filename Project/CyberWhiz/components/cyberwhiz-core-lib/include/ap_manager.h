/**
 * @file ap_manager.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2024-08-02
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

#ifndef AP_MANAGER_H_
#define AP_MANAGER_H_

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "regex.h"
#include "esp_wifi_types_generic.h"

#define DEFAULT_SCAN_LIST_SIZE 50

#define ESP_AP_CONNECT_MAXIMUM_RETRY_ONBOARD  4
#define ESP_AP_CONNECT_MAXIMUM_RETRY_GENERAL  1
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

#define WIFI_RECONNECT_TIMEOUT_IN_SECS 120

extern wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
extern uint16_t ap_count;
extern uint16_t numberAP;

esp_err_t wifi_init();
void wifi_scan(void);
void wifi_init_sta(const char* ssid, const char* pw);
void wifi_check_task(void *pvParameters);
void wifi_disconnect(void);
void wifiReconnectCallback(TimerHandle_t xTimer);

#endif /* AP_MANAGER_H_ */
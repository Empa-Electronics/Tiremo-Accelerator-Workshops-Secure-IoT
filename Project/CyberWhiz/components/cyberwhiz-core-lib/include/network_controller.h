/**
 * @file network_controller.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2024-08-01
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

#ifndef NETWORK_CONTROLLER_H_
#define NETWORK_CONTROLLER_H_

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include "freertos/timers.h"
#include <string.h>

#define ENVIRONMENT "PROD"
#define PROD_OTA_SERVER
// #define DEV_OTA_SERVER

#define NETWORK_CONTROLLER_STATE_MACHINE_QUEUE_SIZE 40
#define NETWORK_CONTROLLER_TASK_STACK_SIZE 8192
#define EVENT_DATA_MAX_SIZE 256

#define ONBOARD_TIMEOUT_IN_SECS 600

#define FUNCTION_CONTROL_ARRAY_SIZE 34
#define CONFIGURATION_ARRAY_SIZE 21

extern uint8_t functionControlArrayData[FUNCTION_CONTROL_ARRAY_SIZE];
extern uint8_t configurationArrayData[CONFIGURATION_ARRAY_SIZE];

extern QueueHandle_t eventQueueHandle;
extern int32_t isDeviceOnboarded;

extern uint16_t descaleDay;

typedef enum {
    STATE_INIT,
    STATE_CHECK_ONBOARD,
    STATE_ONBOARDING_PROCESS,
    STATE_FACTORY_RESET,
    STATE_CONNECT_WIFI,
    STATE_CONNECT_MQTT,
    STATE_NORMAL_OPERATING_MODE,
    STATE_MB_OTA,
    STATE_ERROR
} state_t;

typedef enum {
    EVENT_SOURCE_NETWORK_CONTROLLER,
    EVENT_SOURCE_BLE,
    EVENT_SOURCE_UART,
    EVENT_SOURCE_AP_MANAGER,
    EVENT_SOURCE_MQTT_HANDLER,
    EVENT_SOURCE_OTA_HANDLER,
    EVENT_SOURCE_SNTP_HANDLER,
} source_t;

typedef enum {
    NO_INFO,
    AP_CONNECT_FAIL,
    MQTT_CONNECT_FAIL,
    MQTT_CONNECT_SUCCESS,
} onboard_status_t;

typedef enum {
    EVENT_ENTRY,
    EVENT_CONNECT_WIFI,
    EVENT_DISCONNECT_WIFI,
    EVENT_CONNECT_MQTT,
    EVENT_DISCONNECT_MQTT,
    EVENT_RECONNECT_MQTT,
    EVENT_MQTT_CONNECTED,
    EVENT_UART_DATA_RECEIVED,
    EVENT_BLE_DATA_RECEIVED,
    EVENT_BLE_CONNECTED,
    EVENT_BLE_DISCONNECTED,
    EVENT_ONBOARDING_MODE_ACTIVATED,
    EVENT_ONBOARDING_MODE_DISABLED,
    EVENT_MAINBOARD_INFO_RECEIVED,
    EVENT_MQTT_RPC_REQUEST,

    ///< BLE Onboarding Events
    EVENT_KEY_EXCHANGE,
    EVENT_ACCESS_POINT_COUNT_REQUESTED,
    EVENT_ACCESS_POINT_SSID_REQUESTED,
    EVENT_CONNECT_CREDENTIALS_RECEIVED,
    EVENT_CHECK_AP_CONNECTION_STATUS,
    EVENT_NOTIFY_MOBILE_ONBOARD_STATUS,

    ///< MB OTA Events
    EVENT_MB_OTA_DOWNLOAD_EVENT,
    EVENT_MB_OTA_START_EVENT,
    EVENT_MB_OTA_IMAGE_CHUNK_TRANSFER,
    EVENT_MB_OTA_TRANSFER_COMPLETED,
    EVENT_MB_OTA_COMPARE_CHECKSUM,
    EVENT_MB_RESETTED_FOR_UPDATE,
    EVENT_MB_OTA_ABORT,
} eventType_t;

typedef struct {
    eventType_t eventType;
    source_t source;
    char data[EVENT_DATA_MAX_SIZE];
    size_t dataSize;
} event_t;

void network_controller_task (void *pvParameters);
int send_event_to_queue (eventType_t eventType, source_t source, const char *data, size_t dataSize);
void periodicMainboardInfoQueryTimerCallback (TimerHandle_t xTimer);
void periodicDescaleInfoQueryTimerCallback (TimerHandle_t xTimer);
void secureMercekCallback (TimerHandle_t xTimer);
void mbOTATimeoutCallback (TimerHandle_t xTimer);
void mqttConnectTimeoutCallback (TimerHandle_t xTimer);
void onboardTimeoutCallback (TimerHandle_t xTimer);
const char *getBoardVersionStr (int boardType);

#endif /* NETWORK_CONTROLLER_H_ */
/**
 * @file secure_mercek.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @author Metin Önal (metin.onal@cyberwhiz.co.uk)
 * @brief 
 * @date 2025-08-17
 * 
 * @copyright, Copyright (C) 2025 by CyberWhiz
 * 
 *    All Rights Reserved.
 *    Permission to use, reproduce, copy, prepare derivative works,
 *    modify, distribute, perform, display or sell this software and/or
 *    its documentation for any purpose is prohibited without the express
 *    written consent of CyberWhiz
 * 
 */

#ifndef SECURE_MERCEK_H_
#define SECURE_MERCEK_H_

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include <stdbool.h>

#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <esp_wifi.h>
#include <errno.h>
#include <unistd.h>
#include "cJSON.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define COMPANY_NAME_EMPA 13
#define CLIENT_TYPE_EMPA 7 ///< check with defence center
#define DEVICE_TYPE_EMPA 8 ///< check with defence center
#define OPERATING_SYSTEM_EMPA 6 ///< FreeRTOS

#define MAX_PORTS 1
#define MAX_LEN_PENDING_CONNECTIONS 5

#define SCAN_DEBOUNCE_MS 2000   // ms
#define SCAN_DEBOUNCE_BUCKETS 4 // remember Ip count

#define SECURE_MERCEK_SEND_PERIOD_IN_SECS 5

#define MQTT_UP_TIMER_PERIOD 60

typedef struct {
	int port;
	int sockfd;
} monitored_port_t;

typedef struct {
    uint32_t ip_be;   // client_addr.sin_addr.s_addr (network byte order)
    uint32_t last_ms; // last seen (ms)
} ip_seen_t;

typedef enum {
    WIFI_SEC_OPEN = 0,            // Open
    WIFI_SEC_WEP = 1,             // WEP
    WIFI_SEC_WPA_PSK = 2,         // WPA_PSK
    WIFI_SEC_WPA2_PSK = 3,        // WPA2_PSK
    WIFI_SEC_WPA_WPA2_PSK = 4,    // WPA_WPA2_PSK
    WIFI_SEC_WPA2_ENTERPRISE = 5, // WPA2_ENTERPRISE
    WIFI_SEC_WPA3_PSK = 6,        // WPA3_PSK
    WIFI_SEC_WPA2_WPA3_PSK = 7,   // WPA2_WPA3_PSK
    WIFI_SEC_WAPI_PSK = 8,        // WAPI_PSK
    WIFI_SEC_OWE = 9              // OWE
} mercek_wifi_security_t;

typedef enum {
	PORT_TRACKING_OK = 0,
	PORT_TRACKING_WIFI_DISCONNECTED,
	PORT_TRACKING_ALREADY_RUNNING,
	PORT_TRACKING_SOCKET_OP_FAILED,
	PORT_TRACKING_ALREADY_LISTENING,
	PORT_TRACKING_TASK_CREATE_FAIL,
	PORT_TRACKING_UNKNOWN_ERROR
} PORT_TRACKING_STATUS_t;

typedef struct __attribute__((packed)) {
    uint8_t wifiRSII;
    uint8_t wifiSecurity;
    uint8_t durationLastConnectionHigh;
    uint8_t durationLastConnectionLow;
    uint8_t succOnboardCntHigh;
    uint8_t succOnboardCntLow;
    uint8_t failOnboardCntHigh;
    uint8_t failOnboardCntLow;
    uint8_t succTLSConnCntHigh;
    uint8_t succTLSConnCntLow;
    uint8_t failTLSConnCntHigh;
    uint8_t failTLSConnCntLow;
    uint8_t succOTACntHigh;
    uint8_t succOTACntLow;
    uint8_t failOTACntHigh;
    uint8_t failOTACntLow;
    uint8_t portScanCntHigh;
    uint8_t portScanCntLow;
    uint8_t companyName;
    uint8_t clientType;
    uint8_t deviceType;
    uint8_t firmwareVersionMajor;
    uint8_t firmwareVersionMinor;
    uint8_t firmwareVersionBuild;
    uint8_t operatingSystem;
    uint8_t lastResetReason[5];
    uint8_t connBoardPowerCycleCntLow;
    uint8_t connBoardPowerCycleCntHigh;
    uint8_t succFactoryResetCntLow;
    uint8_t succFactoryResetCntHigh;
    uint8_t failFactoryResetCntLow;
    uint8_t failFactoryResetCntHigh;
} secureMercekStruct_t;

extern secureMercekStruct_t secureMercekArr;
extern uint16_t durationLastConnectionMins;

///< Functions
esp_err_t init_secure_mercek();
esp_err_t reset_secure_mercek();
void increaseSuccOnboardCnt();
void increaseFailOnboardCnt();
void increaseSuccTLSConnCnt();
void increaseFailTLSConnCnt();
void increaseSuccOTACnt();
void increaseFailOTACnt();
void incrasePortScanCnt();
void increaseSuccFactoryResetCnt();

bool sm_should_count_scan(uint32_t ip_be);

///< Port scan check
PORT_TRACKING_STATUS_t socket_operations(void);
PORT_TRACKING_STATUS_t start_port_tracking_task(void);
void stop_port_tracking_task(void);
bool is_wifi_connected(void);

esp_err_t update_secure_mercek_dynamic_data();
esp_err_t store_secure_mercek_to_nvs(void);
char* create_secure_mercek_json(void);
esp_err_t send_secure_mercek();

esp_err_t mercek_get_current_rssi(uint8_t *out_rssi);
esp_err_t mercek_get_current_wifi_security(mercek_wifi_security_t *out_sec);

#endif /* SECURE_MERCEK_H_ */
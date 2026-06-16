/**
 * @file ble_gatts.h
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

#ifndef BLE_GATTS_H_
#define BLE_GATTS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "esp_err.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gatt_common_api.h"
#include "esp_mac.h"

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

extern uint8_t bleConnectionStatus;

extern uint8_t peerPubKey[64];
extern uint8_t sharedSecret[32];
extern bool isSharedSecretCalculated;


// typedef struct blePacket
// {
//     uint8_t preamble;
//     uint8_t packetID;
//     union data
//     {
//         uint8_t ssidNo;
//         uint8_t apCnt;
//     }data_u;
//     uint8_t ssid[33];
// } blePacket_t;

#define GATTS_SERVICE_UUID_A   0x00FF
#define GATTS_CHAR_UUID_A      0xFF01
#define GATTS_DESCR_UUID_A     0x3333
#define GATTS_NUM_HANDLE_A     4

#define BLE_ENCRYPTION

void write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void sendDataOverBLE(const char* data, uint16_t dataSize);
void updateBLEAdvDeviceName(uint8_t isOnboardActive);
void bleTimeoutCallback(TimerHandle_t xTimer);
esp_err_t ble_init();

#define MANUFACTURER_DATA_LEN  6

#define GATTS_CHAR_VAL_LEN_MAX 0x40

#define PREPARE_BUF_MAX_SIZE 256

#define BLE_IDLE_TIMEOUT_IN_SECS 1200

#define PROFILE_NUM 1
#define PROFILE_A_APP_ID 0

#define MINIMUM_BLE_PACKET_SIZE 2
#define MINIMUM_BLE_ENCRYPTED_PACKET_SIZE 30

#define GATTS_TAG "CYBERWHIZ GATTS"

#endif /* BLE_GATTS_H_ */
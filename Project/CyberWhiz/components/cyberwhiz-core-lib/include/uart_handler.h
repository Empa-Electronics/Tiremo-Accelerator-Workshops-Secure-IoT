/**
 * @file uart_handler.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2024-08-05
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

#ifndef UART_HANDLER_H_
#define UART_HANDLER_H_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "string.h"

#define TXD_PIN (18)
#define RXD_PIN (19)
#define RTS_PIN (UART_PIN_NO_CHANGE)
#define CTS_PIN (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

#define UART_PORT_NUM      (2)
#define UART_BAUD_RATE     (115200)
#define UART_TASK_STACK_SIZE    (6144)

typedef enum {
    CMD_GET_MAINBOARD_VERSION = 0x00,
    CMD_INFO_MAINBOARD_VERSION,
    CMD_ACK,
    CMD_GET_FUNCTION_CONTROL_ARRAY,
    CMD_SET_FUNCTION_CONTROL_ARRAY,
    CMD_INFO_FUNCTION_CONTROL_ARRAY,
    CMD_GET_CONFIGURATION_ARRAY,
    CMD_SET_CONFIGURATION_ARRAY,
    CMD_INFO_CONFIGURATION_ARRAY,
    CMD_INFO_TELEMETRY_CRITICAL,
    CMD_INFO_TELEMETRY_NONCRITICAL,
    CMD_GET_TIMER,
    CMD_SET_TIMER,
    CMD_INFO_TIMER,
    CMD_SET_AND_START_USER_WASH,
    CMD_SET_BLE_ONBOARDING,

    /// @brief MB OTA Commands
    CMD_MB_OTA_UPDATE_AVAILABLE,
    CMD_MB_READY_FOR_IMAGE_TRANSFER,
    CMD_MB_NOT_READY_FOR_IMAGE_TRANSFER,
    CMD_MB_IMAGE_CHUNK_TRANSFER,
    CMD_MB_OTA_IMAGE_TRANSFER_COMPLETED,
    CMD_MB_OTA_IMAGE_CHECKSUM,
    CMD_MB_OTA_IMAGE_TRANSFER_SUCCESS,
    CMD_MB_OTA_IMAGE_TRANSFER_FAIL,
    CMD_MB_RESETTING_FOR_UPDATE,
    CMD_MB_OTA_ABORT,

    CMD_GET_DESCALE_INFO,
    CMD_DESCALE_INFO,
    CMD_SET_ECO_DURATION,
    CMD_SET_NIGHTLIGHT_DURATION,

    /// @brief Only BLE Commands
    CMD_GET_DESCALE_INFO_BLE = 0xF8,
    CMD_GET_MAC_ID = 0xF9,
    CMD_GET_PEER_PUB_KEY = 0xFA,
    CMD_SEND_CONNECTION_STATUS = 0xFB, ///< Only BLE commands
    CMD_GET_CONNECT_CREDENTIALS,
    CMD_GET_ACCESS_POINT_SSIDS,
    CMD_GET_ACCESS_POINT_COUNT,
    CMD_GET_PRODUCT_INFO,
} command_types_t;

#define SIZE_OF_COMMAND_TYPE 1

#define UART_BUF_SIZE (1024)

#define PREAMBLE 0x55AA
#define VERSION 0x01

#define MINIMUM_ACCEPTABLE_FRAME_SIZE 8
#define MINIMUM_PACKET_ID_FOR_NETWORK_BOARD 128

#define MAX_UART_DATA_LENGTH 256

#pragma pack(push, 1)
typedef struct {
    uint8_t boardType;
    uint8_t mainBoardVersionHardware;
    uint8_t mainBoardVersionBootloader;
    uint8_t mainBoardVersionSoftware;
    uint8_t sideButtonBoard1VersionHardware;
    uint8_t sideButtonBoard1VersionBootloader;
    uint8_t sideButtonBoard1VersionSoftware;
    uint8_t sideButtonBoard2VersionHardware;
    uint8_t sideButtonBoard2VersionBootloader;
    uint8_t sideButtonBoard2VersionSoftware;
    uint8_t microwaveSensorBoardVersionHardware;
    uint8_t microwaveSensorBoardVersionBootloader;
    uint8_t microwaveSensorBoardVersionSoftware;
    uint8_t waterLevelSensorBoardVersionHardware;
    uint8_t waterLevelSensorBoardVersionBootloader;
    uint8_t waterLevelSensorBoardVersionSoftware;
    uint8_t networkBoardVersionMajor;
    uint8_t networkBoardVersionMinor;
    uint8_t networkBoardVersionBuild;
} tsProductInfo;
#pragma pack(pop)

extern tsProductInfo productInfo;

extern uint8_t ackPacketID;
extern uint8_t packetID;

esp_err_t initUART(void);
int sendUARTFrame(uint8_t version, command_types_t command_type, uint8_t *command_data, uint16_t length);

int queryMainboardVersionsOverUART();
int queryDescaleInfoOverUART();
int queryFunctionControlArrayOverUART();
int updateFunctionControlArrayOverUART(uint8_t *indexValuePairs, uint16_t length);
int queryConfigurationArrayOverUART();
int queryTimerOverUART();
int updateConfigurationArrayOverUART(uint8_t *indexValuePairs, uint16_t length);
int notifyMBforOTAUpdate(uint8_t MBNewHardwareVersion, uint8_t MBNewBootloaderVersion, uint8_t MBNewSoftwareVersion, uint32_t imageSize);

#endif /* UART_HANDLER_H_ */
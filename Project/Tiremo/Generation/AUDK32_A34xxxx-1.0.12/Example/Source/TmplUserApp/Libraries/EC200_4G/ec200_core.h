/**
 * @file    ec200_core.h
 * @brief   Platform-independent EC200 Quectel 4G LTE modem driver - Core API
 *
 * AT command set is compatible with Quectel BG95 series.
 * NO platform-specific (HAL, register, GPIO) dependencies allowed here.
 */

#ifndef EC200_CORE_H
#define EC200_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "ec200_port.h"

#include "../../config/board_config.h"

#define EC200_RX_BUF_SIZE       4096U
#define EC200_MQTT_MSG_MAX      560U

#define EC200_PWRKEY_PORT       BOARD_EC200_PWRKEY_PORT
#define EC200_PWRKEY_PIN        BOARD_EC200_PWRKEY_PIN
#define EC200_PWR_PORT          BOARD_EC200_PWR_PORT
#define EC200_PWR_PIN           BOARD_EC200_PWR_PIN

typedef enum
{
    EC200_STATE_IDLE = 0,
    EC200_STATE_POWER_ENABLE,
    EC200_STATE_CHECK_READY,
    EC200_STATE_CHECK_AT,
    EC200_STATE_CHECK_CPIN,
    EC200_STATE_CFG_RAT_SRCH_SEQ,
    EC200_STATE_CFG_NWSCANMODE,
    EC200_STATE_CHECK_CREG,
    EC200_STATE_SET_APN,
    EC200_STATE_ENABLE_QIACT,
    EC200_STATE_SSL_CFG,
    EC200_STATE_MQTT_OPEN,
    EC200_STATE_MQTT_CONNECT,
    EC200_STATE_GET_TIMESTAMP,
    EC200_STATE_GET_IMEI,
    EC200_STATE_MQTT_PUBLISH,
    EC200_STATE_MQTT_DISCONNECT,
    EC200_STATE_POWER_OFF,
    EC200_STATE_RESET,
    EC200_STATE_DONE,
    EC200_STATE_ERROR
} EC200_State_t;

typedef enum
{
    EC200_OK = 0,
    EC200_ERROR,
    EC200_TIMEOUT
} EC200_Status_t;

extern EC200_State_t     ec200_state;
extern uint8_t           ec200_rx_buf[EC200_RX_BUF_SIZE];
extern volatile uint16_t ec200_rx_len;
extern uint32_t          ec200_unix_timestamp;
extern char              ec200_timestamp_str[32];
extern char              ec200_imei[16];

void EC200_Init(void);
void EC200_SendCmd(const char *cmd);
uint8_t EC200_CheckResponse(const char *expected, uint32_t timeout_ms);
void EC200_ResetRxBuffer(void);
void EC200_PowerOn(void);
void EC200_PowerOff(void);
void EC200_Reset(void);
EC200_Status_t EC200_RunStateMachine(void);
EC200_Status_t EC200_PublishSensorData(const char *topic, const char *data);
EC200_Status_t EC200_GetTimestamp(void);
EC200_Status_t EC200_GetIMEI(void);
void EC200_UART_RxCallback(void);

#ifdef __cplusplus
}
#endif

#endif /* EC200_CORE_H */

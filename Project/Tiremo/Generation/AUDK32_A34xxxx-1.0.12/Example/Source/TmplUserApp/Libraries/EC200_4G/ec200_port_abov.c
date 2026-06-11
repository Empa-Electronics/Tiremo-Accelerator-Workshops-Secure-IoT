/**
 * @file    ec200_port_abov.c
 * @brief   ABOV MCU (A34G43x) implementation of the EC200 port interface
 */

#include "ec200_port.h"
#include "../../config/board_config.h"
#include <string.h>

#include "hal_uart.h"
#include "hal_pcu.h"
#include "system_a34xxxx.h"

#define EC200_UART_ID           BOARD_EC200_UART_ID
#define EC200_DEBUG_UART_ID     UART_ID_0

static volatile uint32_t s_tick_ms = 0;

void Ec200Port_ABOV_TickIncrement(void)
{
    s_tick_ms++;
}

static Ec200Port_Status abov_uart_transmit(const uint8_t *data, uint16_t len, uint32_t timeout)
{
    (void)timeout;

    HAL_ERR_e status = HAL_UART_Transmit(EC200_UART_ID, (uint8_t *)data, (uint32_t)len, true);

    switch (status) {
        case HAL_ERR_OK:      return EC200_PORT_OK;
        case HAL_ERR_BUSY:    return EC200_PORT_BUSY;
        case HAL_ERR_TIMEOUT: return EC200_PORT_TIMEOUT;
        default:              return EC200_PORT_ERROR;
    }
}

static Ec200Port_Status abov_uart_receive_it(uint8_t *buffer, uint16_t len)
{
    HAL_ERR_e status = HAL_UART_Receive(EC200_UART_ID, buffer, (uint32_t)len, false);

    switch (status) {
        case HAL_ERR_OK:   return EC200_PORT_OK;
        case HAL_ERR_BUSY: return EC200_PORT_BUSY;
        default:           return EC200_PORT_ERROR;
    }
}

static Ec200Port_Status abov_uart_abort_receive_it(void)
{
    HAL_ERR_e status = HAL_UART_Abort(EC200_UART_ID);
    return (status == HAL_ERR_OK) ? EC200_PORT_OK : EC200_PORT_ERROR;
}

static void abov_delay_ms(uint32_t ms)
{
    SystemDelayMS(ms);
}

static uint32_t abov_get_tick_ms(void)
{
    return s_tick_ms;
}

static void abov_gpio_init(void)
{
    HAL_PCU_SetInOutMode(BOARD_EC200_PWRKEY_PORT, BOARD_EC200_PWRKEY_PIN,
                         PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetAltMode(BOARD_EC200_PWRKEY_PORT, BOARD_EC200_PWRKEY_PIN, PCU_ALT_0);
    HAL_PCU_SetOutputBit(BOARD_EC200_PWRKEY_PORT, BOARD_EC200_PWRKEY_PIN, PCU_OUTPUT_BIT_SET);

    HAL_PCU_SetInOutMode(BOARD_EC200_PWR_PORT, BOARD_EC200_PWR_PIN,
                         PCU_INOUT_OUTPUT_PUSH_PULL);
    HAL_PCU_SetAltMode(BOARD_EC200_PWR_PORT, BOARD_EC200_PWR_PIN, PCU_ALT_0);
    HAL_PCU_SetOutputBit(BOARD_EC200_PWR_PORT, BOARD_EC200_PWR_PIN, PCU_OUTPUT_BIT_SET);
}

static void abov_gpio_write(Ec200Port_GpioPin pin, Ec200Port_GpioState state)
{
    PCU_ID_e port;
    PCU_PIN_ID_e pin_id;
    PCU_OUTPUT_BIT_e bit;

    if (pin == EC200_GPIO_PWRKEY) {
        port = BOARD_EC200_PWRKEY_PORT;
        pin_id = BOARD_EC200_PWRKEY_PIN;
        bit = (state == EC200_GPIO_HIGH) ? PCU_OUTPUT_BIT_SET : PCU_OUTPUT_BIT_CLEAR;
    } else if (pin == EC200_GPIO_PWR) {
        port = BOARD_EC200_PWR_PORT;
        pin_id = BOARD_EC200_PWR_PIN;
        /* Hardware: CLEAR = power ON, SET = power OFF */
        bit = (state == EC200_GPIO_HIGH) ? PCU_OUTPUT_BIT_CLEAR : PCU_OUTPUT_BIT_SET;
    } else {
        return;
    }

    HAL_PCU_SetOutputBit(port, pin_id, bit);
}

static void abov_debug_print(const char *msg)
{
    HAL_UART_Transmit(EC200_DEBUG_UART_ID, (uint8_t *)msg, (uint32_t)strlen(msg), true);
}

static const Ec200Port_Interface abov_port = {
    .uart_transmit         = abov_uart_transmit,
    .uart_receive_it       = abov_uart_receive_it,
    .uart_abort_receive_it = abov_uart_abort_receive_it,
    .delay_ms              = abov_delay_ms,
    .get_tick_ms           = abov_get_tick_ms,
    .gpio_init             = abov_gpio_init,
    .gpio_write            = abov_gpio_write,
    .debug_print           = abov_debug_print,
};

void Ec200Port_ABOV_Init(void)
{
    Ec200Port_Init(&abov_port);
}

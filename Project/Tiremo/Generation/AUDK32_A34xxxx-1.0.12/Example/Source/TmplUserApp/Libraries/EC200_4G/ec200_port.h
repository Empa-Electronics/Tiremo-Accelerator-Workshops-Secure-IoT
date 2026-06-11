/**
 * @file    ec200_port.h
 * @brief   Platform Abstraction Layer (PAL) for EC200 4G Library
 *
 * Hardware abstraction interface for the EC200 Quectel modem driver.
 * Core logic calls only functions defined here, keeping the library
 * platform-independent.
 *
 * To port to a new MCU:
 *   1. Include this header
 *   2. Implement all functions in Ec200Port_Interface
 *   3. Call Ec200Port_Init() with your implementation before using EC200
 */

#ifndef EC200_PORT_H
#define EC200_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef enum {
    EC200_PORT_OK       = 0x00U,
    EC200_PORT_ERROR    = 0x01U,
    EC200_PORT_BUSY     = 0x02U,
    EC200_PORT_TIMEOUT  = 0x03U
} Ec200Port_Status;

typedef enum {
    EC200_GPIO_PWRKEY = 0,
    EC200_GPIO_PWR,
    EC200_GPIO_PIN_MAX
} Ec200Port_GpioPin;

typedef enum {
    EC200_GPIO_LOW  = 0,
    EC200_GPIO_HIGH = 1
} Ec200Port_GpioState;

typedef struct {
    Ec200Port_Status (*uart_transmit)(const uint8_t *data, uint16_t len, uint32_t timeout);
    Ec200Port_Status (*uart_receive_it)(uint8_t *buffer, uint16_t len);
    Ec200Port_Status (*uart_abort_receive_it)(void);
    void (*delay_ms)(uint32_t ms);
    uint32_t (*get_tick_ms)(void);
    void (*gpio_init)(void);
    void (*gpio_write)(Ec200Port_GpioPin pin, Ec200Port_GpioState state);
    void (*debug_print)(const char *msg);
} Ec200Port_Interface;

void Ec200Port_Init(const Ec200Port_Interface *port);
const Ec200Port_Interface* Ec200Port_Get(void);

#ifdef __cplusplus
}
#endif

#endif /* EC200_PORT_H */

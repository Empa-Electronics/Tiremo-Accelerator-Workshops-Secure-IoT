/**
 *******************************************************************************
 * @file        prv_user_code.c
 * @author      ABOV R&D Division
 * @brief       Tiremo LED Blink — all user LEDs on/off every 1 second
 *
 * Copyright 2024 ABOV Semiconductor Co.,Ltd. All rights reserved.
 *
 * This file is licensed under terms that are found in the LICENSE file
 * located at Document directory.
 * If this file is delivered or shared without applicable license terms,
 * the terms of the BSD-3-Clause license shall be applied.
 * Reference: https://opensource.org/licenses/BSD-3-Clause
 ******************************************************************************/

#include "abov_config.h"
#include "abov_module_config.h"
#include "hal_pcu.h"
#include <stdbool.h>

#if (CONFIG_DEBUG == 1)
#include "debug_log.h"
#include "debug.h"
#endif

/* Tiremo board LEDs — active-low: CLEAR = on, SET = off */
typedef struct
{
    PCU_ID_e     port;
    PCU_PIN_ID_e pin;
} LedPin_t;

static const LedPin_t s_leds[] =
{
    { PCU_ID_B, PCU_PIN_ID_4  },  /* LED1  — PB4  */
    { PCU_ID_B, PCU_PIN_ID_9  },  /* LED2  — PB9  */
    { PCU_ID_F, PCU_PIN_ID_7  },  /* LED3  — PF7  */
    { PCU_ID_B, PCU_PIN_ID_5  },  /* LED4  — PB5  */
    { PCU_ID_B, PCU_PIN_ID_10 },  /* LED5  — PB10 */
    { PCU_ID_B, PCU_PIN_ID_11 },  /* LED6  — PB11 */
    { PCU_ID_B, PCU_PIN_ID_12 },  /* LED7  — PB12 */
    { PCU_ID_B, PCU_PIN_ID_13 },  /* LED8  — PB13 */
    { PCU_ID_B, PCU_PIN_ID_14 },  /* LED9  — PB14 */
    { PCU_ID_B, PCU_PIN_ID_15 },  /* LED10 — PB15 */
};

#define LED_COUNT           ((uint32_t)(sizeof(s_leds) / sizeof(s_leds[0])))
#define LED_BLINK_PERIOD_MS 1000U

extern uint32_t SystemCoreClock;

static volatile uint32_t s_tickMs = 0U;

/**********************************************************************
 * @brief       SysTick handler — 1 ms tick for delay
 **********************************************************************/
void SysTick_Handler(void)
{
    if (s_tickMs > 0U)
    {
        s_tickMs--;
    }
}

/**********************************************************************
 * @brief       Blocking delay in milliseconds
 **********************************************************************/
static void DelayMs(uint32_t ms)
{
    s_tickMs = ms;
    while (s_tickMs > 0U)
    {
        ;
    }
}

/**********************************************************************
 * @brief       Turn on one LED (active-low)
 **********************************************************************/
static void LedOn(PCU_ID_e port, PCU_PIN_ID_e pin)
{
    HAL_PCU_SetOutputBit(port, pin, PCU_OUTPUT_BIT_CLEAR);
}

/**********************************************************************
 * @brief       Turn off one LED (active-low)
 **********************************************************************/
static void LedOff(PCU_ID_e port, PCU_PIN_ID_e pin)
{
    HAL_PCU_SetOutputBit(port, pin, PCU_OUTPUT_BIT_SET);
}

/**********************************************************************
 * @brief       Configure all LED pins as GPIO outputs
 **********************************************************************/
static void LedInit(void)
{
    uint32_t i;

    for (i = 0U; i < LED_COUNT; i++)
    {
        HAL_PCU_SetAltMode(s_leds[i].port, s_leds[i].pin, PCU_ALT_0);
        LedOff(s_leds[i].port, s_leds[i].pin);
    }
}

/**********************************************************************
 * @brief       Turn all LEDs on or off
 **********************************************************************/
static void LedSetAll(bool on)
{
    uint32_t i;

    for (i = 0U; i < LED_COUNT; i++)
    {
        if (on)
        {
            LedOn(s_leds[i].port, s_leds[i].pin);
        }
        else
        {
            LedOff(s_leds[i].port, s_leds[i].pin);
        }
    }
}

/**********************************************************************
 * @brief       User Code Here — blink all LEDs every 1 second
 **********************************************************************/
void PRV_USER_Code(void)
{
    SysTick_Config(SystemCoreClock / 1000U);
    LedInit();

    while (1)
    {
        LedSetAll(true);
        DelayMs(LED_BLINK_PERIOD_MS);

        LedSetAll(false);
        DelayMs(LED_BLINK_PERIOD_MS);
    }
}

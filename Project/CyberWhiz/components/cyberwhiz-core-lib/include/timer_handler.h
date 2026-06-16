/**
 * @file timer_handler.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2025-01-29
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

#ifndef TIMER_HANDLER_H_
#define TIMER_HANDLER_H_

#include <stdint.h>
#include "freertos/timers.h"

void start_named_timer(const char* timer_name, uint32_t timeout_seconds, void (*callback)(TimerHandle_t xTimer), bool is_periodic);

void stop_named_timer(const char* timer_name);

TimerHandle_t get_named_timer(const char* timer_name);

#endif /* TIMER_HANDLER_H_ */
/**
 * @file sntp_controller.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2024-11-26
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

#ifndef SNTP_CONTROLLER_H_
#define SNTP_CONTROLLER_H_

#include <time.h>

#define TIME_SYNC_PERIOD_IN_SECS 3600

/**
 * @brief Initializes the SNTP service and synchronizes the time.
 * This function must be called once at the start to set up SNTP.
 */
void sntp_initialize(void);

/**
 * @brief Gets the current epoch time.
 * @return The current time as an epoch timestamp (seconds since 1970-01-01 00:00:00 UTC).
 */
time_t sntp_get_epoch_time(void);

void sntp_start_with_timer(int update_interval_seconds);

#endif /* SNTP_CONTROLLER_H_ */
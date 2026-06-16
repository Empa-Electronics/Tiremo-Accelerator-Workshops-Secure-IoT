/**
 * @file utils.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2024-09-02
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

#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdbool.h>
#include "stdint.h"
#include "string.h"

uint8_t calculate_crc8(uint8_t *data, size_t len);
uint32_t combine_bytes_to_decimal(const uint8_t *data, size_t length);
void bytes_to_hex_string(const uint8_t *data, size_t length, char *out_str, size_t out_str_size);
int esp32_uECC_rng(uint8_t *dest, unsigned size);

#endif
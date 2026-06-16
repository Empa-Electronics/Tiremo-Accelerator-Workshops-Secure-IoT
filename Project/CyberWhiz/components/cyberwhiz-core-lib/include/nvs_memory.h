/**
 * @file nvs_flash.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2024-08-01
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

#ifndef NVS_FLASH_H_
#define NVS_FLASH_H_

#include "esp_err.h"

/**
 * @brief Initialize the NVS.
 * 
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t nvs_init();

/**
 * @brief Write an integer value to NVS.
 * 
 * @param key The key associated with the value.
 * @param value The integer value to store.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t write_to_nvs(const char* key, int32_t value);

/**
 * @brief Read an integer value from NVS.
 * 
 * @param key The key associated with the value.
 * @param value A pointer to store the read value.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t read_from_nvs(const char* key, int32_t* value);

/**
 * @brief Write a string to NVS.
 * 
 * @param key The key associated with the string.
 * @param value The string to store.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t write_string_to_nvs(const char* key, const char* value);

/**
 * @brief Read a string from NVS.
 * 
 * @param key The key associated with the string.
 * @param value A buffer to store the read string.
 * @param length The length of the buffer.
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t read_string_from_nvs(const char* key, char* value, size_t length);

esp_err_t write_blob_to_nvs(const char* key, const void* data, size_t len);
esp_err_t read_blob_from_nvs(const char* key, void* out_buf, size_t* inout_len);

#endif /* NVS_FLASH_H_ */
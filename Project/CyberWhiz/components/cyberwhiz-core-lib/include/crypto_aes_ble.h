/**
 * @file crypto_aes_ble.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2025-05-16
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

#ifndef CRYPTO_AES_BLE_H_
#define CRYPTO_AES_BLE_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

bool aes_gcm_encrypt(const uint8_t *plaintext, size_t plaintext_len,
                     const uint8_t *key, const uint8_t *iv, size_t iv_len,
                     uint8_t *ciphertext, uint8_t *tag);

bool aes_gcm_decrypt(const uint8_t *ciphertext, size_t ciphertext_len,
                     const uint8_t *key, const uint8_t *iv, size_t iv_len,
                     const uint8_t *tag,
                     uint8_t *output);


#endif /* CRYPTO_AES_BLE_H_ */
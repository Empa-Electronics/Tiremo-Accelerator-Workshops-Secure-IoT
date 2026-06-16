/**
 * @file hsm_manager.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2025-02-10
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

#ifndef HSM_MANAGER_H_
#define HSM_MANAGER_H_

#include "cryptoauthlib.h"

extern ATCAIfaceCfg cfg_atecc608c_i2c;

ATCA_STATUS initHSM(void);
ATCA_STATUS getHSMID(uint8_t *serialNumber);
ATCA_STATUS getRandomNumber(uint8_t *randomNumber);
ATCA_STATUS getRandomNumberHSM(uint8_t *randomNumber, uint8_t size);
ATCA_STATUS getDivirsifiedKey(uint8_t *divKey);
ATCA_STATUS provisionHSM(void);
ATCA_STATUS authorizeHSM(void);
ATCA_STATUS generateAndStoreDeviceCertificate(void);
ATCA_STATUS writeCertificateToHSM(uint8_t *certData, size_t certSize);
ATCA_STATUS readCertificateFromHSM(uint8_t *certificateDataBuff, size_t bufferSize, size_t *certSize);
ATCA_STATUS publickey_validate(bool validate);
ATCA_STATUS testHSM(void);
int http_post_send_HSM_data();

#define ATECC608C_DEVICE_PRIVATE_KEY_SLOT               0  //< Active **
#define ATECC608C_AUTHENTICATION_KEY_SLOT               1  //< Active **
#define ATECC608C_ECDH_PRIVATE_KEY_1_SLOT               2  //< Active **
#define ATECC608C_DEVICE_CERTIFICATE_SIZE_SLOT          3  //< Active **
#define ATECC608C_ECDH_PRIVATE_KEY_2_SLOT               4  //< Active **
#define ATECC608C_ECDH_KDF_OUTPUT_AES_KEY_SLOT          5  //< Active **
#define ATECC608C_ECDH_PRIVATE_KEY_3_SLOT               6  //< Active **
#define ATECC608C_DIVIRSIFIED_KEY_SLOT                  7  //< Active **
#define ATECC608C_DEVICE_CERTIFICATE_PART_I_SLOT        8  //< Active **
#define ATECC608C_TRANSPORT_IO_PROTECTION_KEY_SLOT      9  //< Active **
#define ATECC608C_PARENT_PUBLIC_KEY_SLOT                10 //< Active **
#define ATECC608C_COMMON_KEY_SLOT                       11 //< Active **
#define ATECC608C_VALIDATED_SECURE_BOOT_KEY_SLOT        12 //< Active **
#define ATECC608C_DEVICE_CERTIFICATE_PART_II_SLOT       13 //< Active **
#define ATECC608C_SIGNER_PUBLIC_KEY_SLOT                14 //< Not in use **
#define ATECC608C_DEVICE_CERTIFICATE_PART_III_SLOT      15 //< Active **

#define ATECC608C_DEVICE_CERTIFICATE_PART_I_SIZE 416
#define ATECC608C_DEVICE_CERTIFICATE_PART_II_SIZE (72 - 8)
#define ATECC608C_DEVICE_CERTIFICATE_PART_III_SIZE (72 - 8)

#endif /* HSM_MANAGER_H_ */
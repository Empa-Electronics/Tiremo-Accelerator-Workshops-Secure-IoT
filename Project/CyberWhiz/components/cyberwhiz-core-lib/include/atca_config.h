#pragma once
/* Cryptoauthlib Configuration File */

#ifndef ATCA_CONFIG_H
#define ATCA_CONFIG_H

/* Include HALS */
#define ATCA_HAL_I2C

#define ATCA_MBEDTLS

// #define ATCA_MBEDTLS
// #define ATCA_USE_RTOS_TIMER 1

/* Included device support */
#define ATCA_ATECC608A_SUPPORT
#define ATCAB_AES_GFM_EN          (DEFAULT_ENABLED)
#define CALIB_AES_EN          (DEFAULT_ENABLED)

// #define ATCA_CRYPTO_SHA2_EN         FEATURE_ENABLED
#define ATCAB_LOCK_EN               FEATURE_ENABLED

#define ATCA_USE_ATCAB_FUNCTIONS

/* \brief How long to wait after an initial wake failure for the POST to
 *         complete.
 * If Power-on self test (POST) is enabled, the self test will run on waking
 * from sleep or during power-on, which delays the wake reply.
 */
#ifndef ATCA_POST_DELAY_MSEC
#define ATCA_POST_DELAY_MSEC 25
#endif

/** Define platform provided functions */
#define ATCA_PLATFORM_MALLOC      malloc
#define ATCA_PLATFORM_FREE        free

#define atca_delay_ms   hal_delay_ms
#define atca_delay_us   hal_delay_us

#endif // ATCA_CONFIG_H

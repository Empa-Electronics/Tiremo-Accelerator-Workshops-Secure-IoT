/**
 * @file    mqtt_device_config.h
 * @brief   MQTT broker, device identity, and TLS settings
 *
 * Both ESP32 and EC200 use the broker/topic/client ID values in this file.
 *
 * Certificate data files: certificates/ (PEM, .inc, .key)
 * Accessor code: Libraries/cert_Lib/mqtt_certs.c
 *   <prefix>_rootCA.pem
 *   <prefix>_certificate.pem
 *   <prefix>_private.key
 *   and generated .inc files from the above
 *
 * MQTT_CERT_FILE_PREFIX (no quotes) must match the PEM/.inc file name prefix.
 */

#ifndef MQTT_DEVICE_CONFIG_H
#define MQTT_DEVICE_CONFIG_H

/* =========================================================================
 * DEVICE IDENTITY
 * ========================================================================= */

#define MQTT_USER_ID                "hungarywp4qj"
/* User / tenant ID on the Tiremo platform.
 * Used in topic paths and as the first part of the client ID. */

#define MQTT_DEVICE_NAME            "Tiremo"
/* Name of this physical device.
 * Used in topic paths and as the second part of the client ID. */

#define MQTT_CLIENT_ID              MQTT_USER_ID "_" MQTT_DEVICE_NAME
/* Client ID sent in the MQTT CONNECT packet.
 * Example: "hungarywp4qj_Tiremo"
 * Must match the device/certificate registered on the broker. */

/* =========================================================================
 * BROKER
 * ========================================================================= */

#define MQTT_BROKER_HOST            "iot.tiremo.ai"
/* MQTT broker address (hostname or IP).
 * ESP32: AT+MQTTCONN, EC200: AT+QMTOPEN. */

#define MQTT_TOPIC_PUB              "pub/" MQTT_USER_ID "/" MQTT_DEVICE_NAME "/telemetry"
/* Publish topic for sensor data.
 * Example: "pub/hungarywp4qj/Tiremo/telemetry" */

#define MQTT_TOPIC_SUB              "sub/" MQTT_USER_ID "/" MQTT_DEVICE_NAME "/telemetry"
/* Subscribe topic (defined for ESP32; may not be actively used).
 * Example: "sub/hungarywp4qj/Tiremo/telemetry" */

#define MQTT_KEEP_ALIVE             60
/* MQTT keep-alive interval in seconds.
 * Broker may disconnect if no traffic within this period. */

/* =========================================================================
 * TLS CERTIFICATES
 * ========================================================================= */

#define MQTT_CERT_FILE_PREFIX       hungarywp4qj_Tiremo
/* Certificate file name prefix — DO NOT ADD QUOTES.
 * File names: hungarywp4qj_Tiremo_rootCA.pem etc.
 * Must match MQTT_CLIENT_ID. Update PEM + .inc files when changing device. */

/* =========================================================================
 * TLS ON / OFF
 * =========================================================================
 *
 * MQTT_USE_TLS_CERTS = 1  (default — Tiremo production)
 *   - Port 8883
 *   - ESP32: certificate upload (AT+SYSMFG), MQTT_TLS_4, SNI
 *   - EC200: QFUPL + QSSLCFG, SSL enabled
 *   - Mutual TLS: broker + client certificate required
 *
 * MQTT_USE_TLS_CERTS = 0  (plain MQTT, for test brokers)
 *   - Port 1883
 *   - Certificate upload skipped, unencrypted TCP MQTT
 *   - ESP32: MQTT_TCP mode
 *   - EC200: AT+QMTCFG="ssl",0,0
 *
 * NOTE: iot.tiremo.ai production broker requires TLS (8883).
 *       For non-SSL connections use a test broker on port 1883
 *       (e.g. local Mosquitto or broker.hivemq.com:1883).
 */

#define MQTT_USE_TLS_CERTS          1

#if MQTT_USE_TLS_CERTS
#define MQTT_BROKER_PORT            8883U
/* Broker port when TLS is enabled (standard MQTT over TLS). */
#else
#define MQTT_BROKER_PORT            1883U
/* Broker port when TLS is disabled (plain MQTT). */
#endif

#ifndef MQTT_BROKER_ADDRESS
#define MQTT_BROKER_ADDRESS         MQTT_BROKER_HOST
/* Broker address for quick-connect check in mqtt_core.c.
 * Usually same as MQTT_BROKER_HOST; leave unchanged. */
#endif

#endif /* MQTT_DEVICE_CONFIG_H */

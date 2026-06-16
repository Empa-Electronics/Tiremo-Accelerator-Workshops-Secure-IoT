/**
 * @file    mqtt_certs.h
 * @brief   Shared AWS IoT / broker TLS certificate accessors (ESP32 + SLM320)
 */

#ifndef MQTT_CERTS_H
#define MQTT_CERTS_H

#include "../../config/mqtt_device_config.h"
#include <stddef.h>

const char *MqttCerts_GetRootCA(void);
const char *MqttCerts_GetClientCert(void);
const char *MqttCerts_GetPrivateKey(void);

size_t MqttCerts_GetRootCALen(void);
size_t MqttCerts_GetClientCertLen(void);
size_t MqttCerts_GetPrivateKeyLen(void);

#endif /* MQTT_CERTS_H */

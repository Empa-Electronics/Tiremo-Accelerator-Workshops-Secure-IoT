/**
 * @file    mqtt_certs.c
 * @brief   TLS certificate data (shared by ESP32 MQTT and SLM320)
 */

#include "mqtt_certs.h"

#include <string.h>

#if MQTT_USE_TLS_CERTS

/* .inc files live in TmplUserApp/certificates/ (relative to this .c file).
 * Fixed names — independent of MQTT_CLIENT_ID / MQTT_DEVICE_NAME. */
#include "../../certificates/mqtt_rootCA.inc"
#include "../../certificates/mqtt_certificate.inc"
#include "../../certificates/mqtt_private.inc"

const char *MqttCerts_GetRootCA(void)      { return s_mqtt_root_ca; }
const char *MqttCerts_GetClientCert(void)  { return s_mqtt_client_cert; }
const char *MqttCerts_GetPrivateKey(void)  { return s_mqtt_private_key; }

size_t MqttCerts_GetRootCALen(void)      { return strlen(s_mqtt_root_ca); }
size_t MqttCerts_GetClientCertLen(void)  { return strlen(s_mqtt_client_cert); }
size_t MqttCerts_GetPrivateKeyLen(void)  { return strlen(s_mqtt_private_key); }

#else

const char *MqttCerts_GetRootCA(void)      { return ""; }
const char *MqttCerts_GetClientCert(void)  { return ""; }
const char *MqttCerts_GetPrivateKey(void)  { return ""; }

size_t MqttCerts_GetRootCALen(void)      { return 0U; }
size_t MqttCerts_GetClientCertLen(void)  { return 0U; }
size_t MqttCerts_GetPrivateKeyLen(void)  { return 0U; }

#endif /* MQTT_USE_TLS_CERTS */

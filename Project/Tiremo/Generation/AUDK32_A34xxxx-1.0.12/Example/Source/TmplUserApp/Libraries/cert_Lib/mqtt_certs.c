/**
 * @file    mqtt_certs.c
 * @brief   TLS certificate data (shared by ESP32 MQTT and EC200)
 */

#include "mqtt_certs.h"

#include <string.h>

#if MQTT_USE_TLS_CERTS

#define MQTT_STR(x)                 #x
#define MQTT_INC_JOIN(a, b)         MQTT_INC_JOIN_IMPL(a, b)
#define MQTT_INC_JOIN_IMPL(a, b)    a##b

/* .inc files live in TmplUserApp/certificates/ (relative to this .c file) */
#define MQTT_INC_PATH(file)         MQTT_STR(../../certificates/file)

#define MQTT_INC_ROOTCA             MQTT_INC_JOIN(MQTT_CERT_FILE_PREFIX, _rootCA.inc)
#define MQTT_INC_CERT               MQTT_INC_JOIN(MQTT_CERT_FILE_PREFIX, _certificate.inc)
#define MQTT_INC_KEY                MQTT_INC_JOIN(MQTT_CERT_FILE_PREFIX, _private.inc)

#include MQTT_INC_PATH(MQTT_INC_ROOTCA)
#include MQTT_INC_PATH(MQTT_INC_CERT)
#include MQTT_INC_PATH(MQTT_INC_KEY)

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

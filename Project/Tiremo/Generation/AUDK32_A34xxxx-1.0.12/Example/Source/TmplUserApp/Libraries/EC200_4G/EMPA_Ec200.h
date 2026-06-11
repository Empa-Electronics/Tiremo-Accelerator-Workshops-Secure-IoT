/**
 * @file    EMPA_Ec200.h
 * @brief   EC200 4G MQTT service layer (application-facing API)
 */

#ifndef EMPA_EC200_H
#define EMPA_EC200_H

#include "ec200_core.h"
#include "../Sensor/sensor.h"
#include "../../config/app_config.h"

#define EC200_MAX_TRY_CONNECT   APP_MQTT_CONNECT_RETRIES

uint8_t EC200_ConnectBroker(void);
uint8_t EC200_PublishSensorDataApp(const SensorData_t *pData);

#endif /* EMPA_EC200_H */

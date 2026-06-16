/**
 * @file telemetry.h
 * @author Anil Doyran (anil.doyran@cyberwhiz.co.uk)
 * @brief 
 * @date 2024-11-20
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

#ifndef TELEMETRY_H_
#define TELEMETRY_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "cJSON.h"

#define MAX_JSON_LENGTH 512

extern const int ncr_tel_info_code_descriptions[];
extern const int cr_tel_info_code_descriptions[];

typedef enum {
    CR_TEL_TYPE_SYSTEM_BOOT_UP_EVENT = 0x01,
    CR_TEL_TYPE_START_DESCALING_EVENT,
    CR_TEL_TYPE_STOP_DESCALING_EVENT,
    CR_TEL_TYPE_START_BOWL_UV_CLEANING_EVENT,
    CR_TEL_TYPE_STOP_BOWL_UV_CLEANING_EVENT,
    CR_TEL_TYPE_START_NOZZLE_UV_CLEANING_EVENT,
    CR_TEL_TYPE_STOP_NOZZLE_UV_CLEANING_EVENT,
    CR_TEL_TYPE_START_NOZZLE_CLEANING_EVENT,
    CR_TEL_TYPE_STOP_NOZZLE_CLEANING_EVENT,
    CR_TEL_TYPE_SESSION_EVENT,
    CR_TEL_TYPE_SESSION_WATER_CONSUMPTION_EVENT,
    CR_TEL_TYPE_SESSION_ENERGY_CONSUMPTION_EVENT,
    CR_TEL_TYPE_IDLE_SEAT_HEATING_ENERGY_CONSUMPTION_EVENT,
    CR_TEL_TYPE_WATER_LEVEL_ERROR_EVENT,
    CR_TEL_TYPE_INLET_SUPPLY_WATER_ERROR_EVENT,
    CR_TEL_TYPE_SEAT_HEATER_TEMPERATURE_ERROR_EVENT,
    CR_TEL_TYPE_SEAT_OPEN_CLOSE_ERROR_EVENT,
    CR_TEL_TYPE_COVER_OPEN_CLOSE_ERROR_EVENT,
    CR_TEL_TYPE_WATER_TEMPERATURE_ERROR_EVENT,
    CR_TEL_TYPE_DRYER_TEMPERATURE_ERROR_EVENT,
    CR_TEL_TYPE_MICROWAVE_ERROR_EVENT,
    CR_TEL_TYPE_PERIOD_TELEMETRY
} critical_telemetry_info_type_t;

typedef enum {
    NCR_TEL_TYPE_START_STOP_WASH = 0x01,
    NCR_TEL_TYPE_SET_WASH_MODE,
    NCR_TEL_TYPE_START_STOP_DRYER,
    NCR_TEL_TYPE_PAIR_REMOTE_CONTROL,
    NCR_TEL_TYPE_SET_CHILD_MODE_WASH,
    NCR_TEL_TYPE_SET_LIGHT_BRIGHTNESS,
    NCR_TEL_TYPE_SET_WATER_PRESSURE,
    NCR_TEL_TYPE_SET_WATER_TEMPERATURE,
    NCR_TEL_TYPE_SET_NOZZLE_POSITION,
    NCR_TEL_TYPE_SET_NIGHT_LIGHT_AUTO_MODE,
    NCR_TEL_TYPE_SET_AMBIENT_LIGHT,
    NCR_TEL_TYPE_SET_BOWL_LIGHT,
    NCR_TEL_TYPE_SET_SEAT_TEMPERATURE,
    NCR_TEL_TYPE_SET_AUTO_DAMPER,
    NCR_TEL_TYPE_SET_DEODORISATION,
    NCR_TEL_TYPE_SET_ENERGY_SAVING_MODE,
    NCR_TEL_TYPE_SET_DEMO_MODE,
    NCR_TEL_TYPE_SET_SILENT_MODE,
    NCR_TEL_TYPE_SET_DRYER_TEMPERATURE,
    NCR_TEL_TYPE_SET_DRYER_DURATION,
    NCR_TEL_TYPE_SET_REAR_WASH_DURATION,
    NCR_TEL_TYPE_SET_FRONT_WASH_DURATION,
    NCR_TEL_TYPE_SET_STANDBY_MODE,
    NCR_TEL_TYPE_PRESENCE_EVENT,
    NCR_TEL_TYPE_START_STOP_WATER_HEATING,
    NCR_TEL_TYPE_START_STOP_SEAT_HEATING,
    NCR_TEL_TYPE_OPEN_CLOSE_COVER,
    NCR_TEL_TYPE_OPEN_CLOSE_SEAT
} non_critical_telemetry_info_type_t;

typedef enum {
    SENSOR_TIMEOUT = 0x00,
    SIDE_PANEL,
    REMOTE_CONTROL,
    CONNECTIVITY_MODULE
} telemetry_trigger_source_t;

char* create_critical_telemetry_json(const char* dataInput, uint16_t size);
char* create_noncritical_telemetry_json(const char* dataInput, uint16_t size);
int get_cr_tel_info_code_description(critical_telemetry_info_type_t type);
int get_ncr_tel_info_code_description(non_critical_telemetry_info_type_t type);
void add_data_to_key_flexible(cJSON *root, const char *key, const char *new_data1, const char *new_data2_string, unsigned int value, int use_string_value);
void add_data_to_key(cJSON *root, const char *key, const char *new_data, unsigned int value);

void telemetry_init();  // Call at boot, loads from NVS
bool telemetry_push(const char *json);
bool telemetry_pop(char *json_out);
bool telemetry_is_empty();
void telemetry_resend_buffer();  // Call when MQTT reconnects

char* createShadowReportPayload(uint16_t descaleDay);

char* createAttributePayload();
char* createShadowSystemInfoPayload();
char* createShadowConfigPayload();
char* createShadowMaintenancePayload();
char* createShadowErrorsPayload();
char* createShadowModesPayload();
char* createShadowSettingsPayload();

#endif /* TELEMETRY_H_ */

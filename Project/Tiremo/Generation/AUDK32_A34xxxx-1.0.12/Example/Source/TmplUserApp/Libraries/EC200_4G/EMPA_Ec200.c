#include "../../config/project_config.h"
#include "EMPA_Ec200.h"
#include <stdio.h>
#include <string.h>

uint8_t EC200_ConnectBroker(void)
{
    uint8_t tryCount = 0;

    while (tryCount < EC200_MAX_TRY_CONNECT)
    {
        ec200_state = EC200_STATE_IDLE;

        while (ec200_state != EC200_STATE_MQTT_PUBLISH &&
               ec200_state != EC200_STATE_ERROR &&
               ec200_state != EC200_STATE_DONE)
        {
            EC200_Status_t ret = EC200_RunStateMachine();
            if (ret == EC200_ERROR && ec200_state != EC200_STATE_RESET)
                break;
        }

        if (ec200_state == EC200_STATE_MQTT_PUBLISH)
            return 0;

        tryCount++;
    }

    return 1;
}

uint8_t EC200_PublishSensorDataApp(const SensorData_t *pData)
{
    static char jsonBuf[256];

    Sensor_FormatJSON(pData, jsonBuf, sizeof(jsonBuf));
    return (EC200_PublishSensorData(MQTT_TOPIC_PUB, jsonBuf) == EC200_OK) ? 0U : 1U;
}

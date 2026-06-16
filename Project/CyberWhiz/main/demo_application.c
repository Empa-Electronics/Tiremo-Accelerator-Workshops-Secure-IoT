#include "ap_manager.h"
#include "nvs_memory.h"
#include "network_controller.h"
#include "mqtt_handler.h"
#include "atca_config.h"
#include "hsm_manager.h"
#include "secure_mercek.h"
#include "sntp_controller.h"
#include "timer_handler.h"
#include "crypto_aes_ble.h"
#include "version.h"

static const char *TAG = "CYBERWHIZ MAIN";

void secureMercekCb (TimerHandle_t xTimer);

#define SECURE_MERCEK_PERIOD_IN_SECS 6
#define DEVICE_NUMBER //< Please fill here with your device ID

QueueHandle_t eventQueueHandle;

/// @brief Security Related Telemetry - IoT Core - HIDS
void app_main(void)
{
    /* -------------------------------------------------------------
     * 1. Print CyberWhiz library version
     *    Helps identify SDK / library compatibility during runtime.
     * ------------------------------------------------------------- */
    ESP_LOGI(TAG,
             "CyberWhiz Library Version: %d.%d.%d",
             NB_VERSION_MAJOR,
             NB_VERSION_MINOR,
             NB_VERSION_BUILD);

    /* -------------------------------------------------------------
     * 2. Initialize NVS (Non-Volatile Storage)
     *    Used for storing configuration and persistent data.
     * ------------------------------------------------------------- */
    ESP_ERROR_CHECK(nvs_init());

    /* -------------------------------------------------------------
     * 3. Initialize Wi-Fi / TCP-IP stack
     *    Prepares network interfaces and event loop.
     * ------------------------------------------------------------- */
    ESP_ERROR_CHECK(wifi_init());

    /* -------------------------------------------------------------
     * 4. Initialize Secure Mercek module
     *
     *    - Initializes security related telemetry buffers
     * ------------------------------------------------------------- */
    ESP_ERROR_CHECK(init_secure_mercek());

    /* -------------------------------------------------------------
     * 5. Set device number
     *
     *    - Sets device number for MQTT Client ID
     * ------------------------------------------------------------- */
    set_device_number(DEVICE_NUMBER);

    /* -------------------------------------------------------------
     * 6. Connect to Wi-Fi Access Point
     *
     *    - Device joins local network
     *    - Required before any cloud or time synchronization
     * ------------------------------------------------------------- */
    wifi_init_sta("AniliPhone", "12345678**");

    /* -------------------------------------------------------------
     * 7. Start SNTP time synchronization
     *
     *    - Required for:
     *        * Accurate telemetry timestamps
     * ------------------------------------------------------------- */
    sntp_start_with_timer(TIME_SYNC_PERIOD_IN_SECS);

    /* -------------------------------------------------------------
     * 8. Establish secure MQTT connection to IoT Core
     *
     *    - Initializes Hardware Security Module (HSM)
     *    - Uses TLS
     *    - Client authentication performed inside HSM
     *    - Private key never leaves secure hardware
     *    - Connects to AWS IoT Core
     * ------------------------------------------------------------- */
    mqtt_connect();

    /* -------------------------------------------------------------
     * 9. Start secure port tracking task
     *
     *    - Monitors network activity
     *    - Detects anomalous behavior
     * ------------------------------------------------------------- */
    start_port_tracking_task();

    /* -------------------------------------------------------------
     * 10. Start Secure Mercek periodic telemetry timer
     *
     *    - Collects security metrics
     *    - Sends secure telemetry to IoT Core at fixed intervals
     * ------------------------------------------------------------- */
    start_named_timer(
        "secureMercekTimer",
        SECURE_MERCEK_PERIOD_IN_SECS,
        secureMercekCb,
        true
    );

    /* -------------------------------------------------------------
     * 10. Keep main task alive
     *
     *    - All functionality runs in background tasks / timers
     *    - app_main remains idle
     * ------------------------------------------------------------- */
    vTaskDelay(portMAX_DELAY);
}

/// @brief Security related telemetry send callback
/// @param xTimer 
void secureMercekCb(TimerHandle_t xTimer){
    (void)xTimer;

    send_secure_mercek();
}
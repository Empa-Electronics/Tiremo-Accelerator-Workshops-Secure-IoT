# Tiremo Connect — TmplUserApp

ABOV A34G43x (Cortex-M) firmware for Tiremo devices. Collects sensor data and publishes it to the **Tiremo MQTT broker** (`iot.tiremo.ai`).

Two independent connectivity paths are supported:

| Path | Module | Connection |
|------|--------|------------|
| **WiFi** | ESP32-C3 (AT firmware) | Local WiFi → MQTT/TLS |
| **4G LTE** | MeiG SLM320 | SIM / PDP → MQTT/TLS |

Both paths share the same broker settings and (optionally) the same TLS certificates.

---

## Project structure

```
TmplUserApp/
├── prv_user_code.c          # Main application loop
├── user_uart_isr.c          # UART ISR (SLM320 on UART1, debug on UART0)
├── config/                  # All settings
│   ├── project_config.h     # Master include
│   ├── app_config.h         # Feature flags, timing
│   ├── board_config.h       # Pins, UART instances
│   ├── network_config.h     # WiFi, APN
│   └── mqtt_device_config.h # Broker, topics, TLS
├── certificates/            # PEM / .inc certificate data
└── Libraries/
    ├── cert_Lib/
    │   └── mqtt_certs.c/.h  # TLS certificate accessors
    ├── MQTT_Library/
    │   ├── mqtt_core.c/.h         # ESP32 AT MQTT state machine
    │   ├── mqtt_port_abov.c       # ABOV UART port layer
    │   └── EMPA_MqttAws.c         # WiFi + MQTT service layer
    ├── MEIG_SLM3XX/
    │   ├── slm320.c/.h            # SLM320 4G modem driver
    │   ├── EMPA_Slm320.c/.h       # 4G + MQTT service layer
    │   ├── README.md              # Driver overview
    │   └── SLM320_AT_Commands.md  # AT command reference
    ├── Sensor/                    # Sensor read + JSON formatting
    ├── SHT40/                     # Temperature / humidity
    ├── LISDE12TR/                 # Accelerometer
    └── MP23ABS1/                  # Microphone
```

---

## Feature selection (`config/app_config.h`)

```c
//#define EMPA_SENSOR_PROCESS   // Print to terminal only
//#define EMPA_ESP32_MQTT_AWS   // ESP32 WiFi + MQTT
#define EMPA_SLM320_4G          // SLM320 4G + MQTT

#define APP_PUBLISH_INTERVAL_MS  2000U
```

| Flag | Behavior |
|------|----------|
| `EMPA_SENSOR_PROCESS` | Print sensor data to debug UART |
| `EMPA_ESP32_MQTT_AWS` | WiFi + MQTT via ESP32 |
| `EMPA_SLM320_4G` | 4G + MQTT via MeiG SLM320 |

ESP32 and SLM320 can be enabled together; they use different UARTs (UART2 vs UART1).

---

## Broker and device settings

### Config files

| File | Contents |
|------|----------|
| `config/project_config.h` | Single include for all config |
| `config/app_config.h` | Feature flags, publish interval |
| `config/board_config.h` | LEDs, I2C, UART, modem pins |
| `config/network_config.h` | WiFi SSID/password, APN |
| `config/mqtt_device_config.h` | Broker, client ID, topics, TLS |

**Broker / device (`mqtt_device_config.h`):**

```c
#define MQTT_USER_ID          "hungarywp4qj"
#define MQTT_DEVICE_NAME      "hun20"

#define MQTT_CLIENT_ID        MQTT_USER_ID "_" MQTT_DEVICE_NAME   // hungarywp4qj_hun20
#define MQTT_BROKER_HOST      "iot.tiremo.ai"

#define MQTT_TOPIC_PUB        "pub/" MQTT_USER_ID "/" MQTT_DEVICE_NAME "/telemetry"
#define MQTT_TOPIC_ALARM      "pub/" MQTT_USER_ID "/" MQTT_DEVICE_NAME "/alarm"

#define MQTT_KEEP_ALIVE       60
#define MQTT_USE_TLS_CERTS    1    // 1 = TLS (8883), 0 = plain MQTT (1883)
```

### Parameter reference

| Define | Example | Used for |
|--------|---------|----------|
| `MQTT_USER_ID` | `hungarywp4qj` | Topic path, client ID prefix |
| `MQTT_DEVICE_NAME` | `hun20` | Topic path, client ID suffix |
| `MQTT_CLIENT_ID` | `hungarywp4qj_hun20` | MQTT CONNECT |
| `MQTT_BROKER_HOST` | `iot.tiremo.ai` | Broker address |
| `MQTT_TOPIC_PUB` | `pub/hungarywp4qj/hun20/telemetry` | Sensor data publish |
| `MQTT_TOPIC_ALARM` | `pub/hungarywp4qj/hun20/alarm` | Alarm publish |
| `MQTT_KEEP_ALIVE` | `60` | MQTT keep-alive (seconds) |
| `MQTT_USE_TLS_CERTS` | `1` | TLS on/off |
| `MQTT_BROKER_PORT` | `8883` / `1883` | Auto-selected from TLS flag |

### Adding a new device / user

1. Change `MQTT_USER_ID` and `MQTT_DEVICE_NAME` in `mqtt_device_config.h`.
2. Download certificates from the broker panel for the new device.
3. Place PEM files under `certificates/` (fixed names):
   - `mqtt_rootCA.pem`
   - `mqtt_certificate.pem`
   - `mqtt_private.key`
4. Regenerate matching `.inc` files for the build (`mqtt_certs.c` includes them).
5. Build and flash.

---

## TLS certificates

### Enable / disable

`mqtt_device_config.h`:

```c
#define MQTT_USE_TLS_CERTS    1   // Certificate-based connection (production)
#define MQTT_USE_TLS_CERTS    0   // Plain MQTT on port 1883 (test brokers only)
```

| Value | ESP32 | SLM320 |
|-------|-------|--------|
| **1** | Cert upload via `AT+SYSMFG`, port 8883 | `QFUPL` + `QSSLCFG` + `QMTOPEN`, port 8883 |
| **0** | Skip cert steps, `MQTT_TCP`, port 1883 | `AT+MQTTCONN` plain API, port 1883 |

> `iot.tiremo.ai` requires TLS. Use plain MQTT only with a local/test broker.

### Certificate files

| File | Description |
|------|-------------|
| `Libraries/cert_Lib/mqtt_certs.c` | Certificate accessor functions |
| `Libraries/cert_Lib/mqtt_certs.h` | `MqttCerts_GetRootCA()` etc. |
| `certificates/*.pem` | Raw certificate source |
| `certificates/*.inc` | Embedded C strings for build |

ESP32 and SLM320 both use `MqttCerts_Get*()` functions.

---

## ESP32 (WiFi + MQTT) flow

```mermaid
sequenceDiagram
    participant App as prv_user_code
    participant EMPA as EMPA_MqttAws
    participant Core as mqtt_core
    participant ESP as ESP32 AT

    App->>EMPA: MQTT_ConnectBroker()
    EMPA->>Core: MQTT_Init()
    Core->>ESP: WiFi connect (CWJAP)
    Core->>ESP: NTP (CIPSNTPCFG)
    Core->>ESP: Cert upload (SYSMFG) [if TLS]
    Core->>ESP: MQTTUSERCFG + MQTTSNI + MQTTCONN
    EMPA->>ESP: QMTPUB (sensor JSON)
```

### WiFi settings (`network_config.h`)

```c
#define WIFI_SSID       "EMPA_Arge"
#define WIFI_PASSWORD   "Emp@Arg2024!"
#define WIFI_TIMEZONE   3
```

### Cellular settings (`network_config.h`)

```c
#define CELLULAR_APN        "internet"
#define CELLULAR_APN_AUTH   0
```

---

## SLM320 (4G + MQTT) flow

```mermaid
sequenceDiagram
    participant App as prv_user_code
    participant Svc as EMPA_Slm320
    participant Drv as slm320.c
    participant Mod as SLM320 Modem

    App->>Svc: SLM320_ConnectBroker()
    Svc->>Drv: SLM320_RunStateMachine() (loop)
    Drv->>Mod: SIM / network / PDP
    Drv->>Mod: TLS certs + QSSLCFG [if TLS]
    Drv->>Mod: QMTOPEN + QMTCONN [TLS] or MQTTCONN [plain]
    App->>Svc: SLM320_PublishSensorDataApp()
    Drv->>Mod: QMTPUBEX [TLS] or MQTTPUB [plain]
```

Detailed AT command reference: **[Libraries/MEIG_SLM3XX/SLM320_AT_Commands.md](Generation/AUDK32_A34xxxx-1.0.12/Example/Source/TmplUserApp/Libraries/MEIG_SLM3XX/SLM320_AT_Commands.md)**

### SLM320 hardware

| Signal | Pin | Description |
|--------|-----|-------------|
| UART | `UART_ID_1` (PA10/PA11) | AT commands, 115200 8N1 |
| PWRKEY | PA7 | LOW ≥ 1 s → module powers on |
| PWR | PC4 | Module power supply enable |

### SLM320 driver logging

The driver logs only high-level milestones and errors on UART0 (debug). Raw modem RX responses are not printed.

Typical successful connect sequence:

```
[SLM320] SIM ready
[SLM320] Registered on network
[SLM320] PDP context active
[SLM320] TLS ready
[SLM320] MQTT broker open
[SLM320] MQTT broker connected
```

---

## Sensors and data format

`Sensor_ReadOnly()` reads all sensors; `Sensor_FormatJSON()` produces JSON.

| Sensor | Measurement |
|--------|-------------|
| SHT40 | Temperature, humidity |
| LIS2DE12 | Acceleration (X/Y/Z) |
| MP23ABS1 | Microphone RMS |
| ADC | Battery voltage |

Example JSON:

```json
{"temp":25.1,"hum":48.2,"bat":3.30,"ax":12,"ay":-5,"az":1001,"mic":1234}
```

Publish interval: **2 seconds** (`APP_PUBLISH_INTERVAL_MS`).

---

## Hardware / UART summary

| UART | Usage |
|------|-------|
| UART0 | Debug terminal |
| UART1 (PA10/PA11) | SLM320 4G modem |
| UART2 (PA8/PA9) | ESP32-C3 AT |

I2C (PB6/PB7): SHT40, LIS2DE12.

---

## Build notes

- Project builds with **Eclipse + GCC** (`Build/Eclipse/TmplUserApp/`).
- `subdir.mk` files are auto-generated by Eclipse.
- Add new `.c` files via Eclipse Project Explorer.
- After structural changes, run **Clean + Build**.

---

## Quick troubleshooting

| Symptom | Likely cause |
|---------|--------------|
| MQTT timeout, cert upload | NTP not complete before TLS (ESP32) |
| CONNACK rejected | `MQTT_CLIENT_ID` does not match broker certificate/device |
| ESP32 WiFi fails | Wrong SSID/password in `network_config.h` |
| SLM320 QIACT error | SIM / APN / signal; APN is `internet` |
| `CME ERROR: 50` | `AT+MQTTCONN` used with TLS — use `QMTOPEN` path |
| `CME ERROR: 58` on publish | Use `AT+QMTPUBEX`, not `AT+QMTPUB` |
| TLS error | `MQTT_USE_TLS_CERTS=1` but certificate missing or expired |
| Port error | TLS on → 8883, off → 1883; must match broker |

---

## Related documents

| File | Contents |
|------|----------|
| [README.md](README.md) | This file — project overview |
| [Libraries/MEIG_SLM3XX/README.md](Generation/AUDK32_A34xxxx-1.0.12/Example/Source/TmplUserApp/Libraries/MEIG_SLM3XX/README.md) | SLM320 driver overview |
| [Libraries/MEIG_SLM3XX/SLM320_AT_Commands.md](Generation/AUDK32_A34xxxx-1.0.12/Example/Source/TmplUserApp/Libraries/MEIG_SLM3XX/SLM320_AT_Commands.md) | SLM320 AT command reference |
| `config/project_config.h` | All settings (master) |
| `config/mqtt_device_config.h` | Broker / device / TLS |

---

*Tiremo Connect v2.0 — ABOV A34G43x*

# Tiremo® Accelerator Workshops — Secure IoT

Welcome to the **Tiremo® Accelerator Workshops** by **Empa Electronics**. This repository contains firmware source code, pre-built binaries, and step-by-step guides for the hands-on workshop.

---

## About Tiremo

**Tiremo** is an end-to-end AIoT infrastructure platform engineered to accelerate the development and deployment of secure connected devices. It provides a robust ecosystem of hardware, software, and cloud tools with a strong focus on cybersecurity, ensuring edge solutions align with strict regulatory frameworks like **RED** and **CRA**. By bridging Edge AI with scalable cloud integration, Tiremo enables developers to seamlessly transition from prototyping to production-ready enterprise applications.

---

## About the Board

This development board is designed by **Empa Electronics** specifically for Edge AI and cloud-IoT applications, and it is delivered with full software support. It is powered by the high-performance **ABOV A34G43ARL2N** microcontroller (ARM® Cortex®-M4F) and features an integrated onboard debugger to streamline your development workflow.

### Hardware specifications

| Feature | Details |
|---------|---------|
| **Sensors** | Analog MEMS microphone, 3-axis accelerometer, temperature/humidity sensors |
| **User interface** | Ten user LEDs and one user button |
| **Connectivity** | Integrated Wi-Fi and Bluetooth LE module |
| **Power & debug** | USB Type-C port (CN6) for power, flashing, and serial debug |

### System architecture

Sensors on the board convert physical stimuli into electronic signals for analysis at the edge or in the cloud. The firmware uses lightweight protocols such as **MQTT** to transmit telemetry reliably over low-bandwidth connections. During this workshop, sensor data is routed to the **Tiremo MQTT broker** (`iot.tiremo.ai`) over **Wi-Fi** or **4G LTE**. Cloud IoT platforms then gather, store, and visualize this data in a centralized system — enabling low-latency, energy-efficient applications when the right sensors and connectivity are chosen at the edge.

---

## Development Environment Setup

Before starting, install the required tools (eMStudio32, MCUBrew32, aFlasher32, Tera Term, etc.):

### ↳ [Development Environment Setup](SetUp.md)

---

## Workshop Activities

After setup, follow the activity guide to open the project, select an application, build, and flash the **Tiremo** board:

### ↳ [1) Data Collection and MQTT Communication with Tiremo](Project/RunningCode.md)

Three workshop applications are included:

| # | Application | Connectivity |
|---|-------------|--------------|
| 1 | Sensor readout to terminal | Debug UART |
| 2 | ESP32 WiFi MQTT | Wi-Fi → `iot.tiremo.ai` |
| 3 | SLM320 4G MQTT | 4G LTE → `iot.tiremo.ai` |

Firmware details: [Project/README.md](Project/README.md)

### ↳ 2) CyberWhiz — Secure IoT on ESP32

ESP-IDF project on **ESP32**: WiFi connection, TLS MQTT, and periodic **Secure Mercek** security telemetry to the cloud. Open `Project/CyberWhiz/` in VS Code with the ESP-IDF extension.

---

## Repository contents

| Folder / file | Description |
|---------------|-------------|
| `Project/` | Tiremo firmware source, Eclipse project, and workshop guide |
| `Binary/` | Pre-built HEX files for all three applications |
| `Abov_SDK/` | AUDK32 SDK archive for MCUBrew32 (`AUDK32_A34xxxx-1.0.12.zip`) |
| `Document/` | Setup guides, screenshots, and reference PDFs |
| `SetUp.md` | Tool installation instructions |

---

<p align="center">
  <sub>© Empa Electronics — Tiremo® Accelerator Workshops</sub>
</p>

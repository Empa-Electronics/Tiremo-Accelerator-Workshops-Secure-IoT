# Running the Project — Workshop Guide

This guide walks you through downloading the Tiremo® Accelerator Workshops repository, opening the firmware project in **eMStudio32**, selecting one of three workshop applications, building and flashing the code to your **Tiremo®Cortex** board, and viewing debug output on a serial terminal.

> **Prerequisites:** Complete the [Development Environment Setup](../SetUp.md) before starting this activity.

---

## Table of Contents

1. [Download the Repository](#1-download-the-repository)
2. [Extract the Project](#2-extract-the-project)
3. [Open the Project in eMStudio32](#3-open-the-project-in-emstudio32)
  - [Launch eMStudio32](#31-launch-emstudio32)
  - [Select Workspace](#32-select-workspace)
  - [Import the Project](#33-import-the-project)
  - [Select Project Directory](#34-select-project-directory)
  - [Configure Device Name](#35-configure-device-name)
  - [Applications](#36-applications)
    - [Application 1 — Sensor Process](#application-1--sensor-process-empa_sensor_process)
    - [Application 2 — ESP32 MQTT](#application-2--esp32-mqtt-empa_esp32_mqtt_aws)
    - [Application 3 — SLM320 4G MQTT](#application-3--slm320-4g-mqtt-empa_slm320_4g)
4. [Flash via Binary (aFlasher32)](#4-flash-via-binary-aflasher32)
5. [View Debug Messages (Tera Term)](#5-view-debug-messages-tera-term)
6. [Configure Peripherals with MCUBrew32 (Optional)](#6-configure-peripherals-with-mcubrew32-optional)

---

## 1. Download the Repository

Clone or download the workshop repository from GitHub:

**Repository:** [https://github.com/Empa-Electronics/Tiremo-Accelerator-Workshops-Secure-IoT](https://github.com/Empa-Electronics/Tiremo-Accelerator-Workshops-Secure-IoT)

You can use **Code → Download ZIP** or clone with Git:

```bash
git clone https://github.com/Empa-Electronics/Tiremo-Accelerator-Workshops-Secure-IoT.git
```

---

## 2. Extract the Project

After downloading, extract the ZIP archive to a folder on your computer. You should see the project structure including the `Project`, `Binary`, and `Document` folders.

Extract the downloaded project

---

## 3. Open the Project in eMStudio32

### 3.1 Launch eMStudio32

Open the **eMStudio32** integrated development environment (IDE).

Open eMStudio32

### 3.2 Select Workspace

When prompted, choose your workspace folder and click **Launch**.

Select workspace and click Launch

### 3.3 Import the Project

In the top toolbar, click the **ABOV** button.

Click the ABOV button in the toolbar

From the menu, select **Open eMStudio32 Project**.

Open eMStudio32 Project

### 3.4 Select Project Directory

In the dialog that opens, click **Directory** and navigate to the Eclipse workspace path for the template user application:

```
..\Tiremo\Generation\AUDK32_A34xxxx-1.0.12\Example\Build\Eclipse\TmplUserApp\Workspace\tmpl_userapp
```

Relative to the extracted `Project` folder, the full path is:

```
Project\Tiremo\Generation\AUDK32_A34xxxx-1.0.12\Example\Build\Eclipse\TmplUserApp\Workspace\tmpl_userapp
```

Click Directory and select the project path

Click **Finish** to import the project.

Click Finish

After import, run **Project → Clean…** (select the **A34G43x** configuration), then **Project → Build Project** once before flashing. This regenerates local build files for your PC.

### 3.5 Configure Device Name

> **Do this before loading or flashing any code** — especially for Application 2 and Application 3.

Each MQTT participant must use a **unique device name** on the broker. Set your number in `mqtt_device_config.h` **before** you build from the IDE or load a HEX file.

Open the MQTT device configuration file:

```
..\Tiremo\Generation\AUDK32_A34xxxx-1.0.12\Example\Source\TmplUserApp\config\mqtt_device_config.h
```

Relative to the extracted `Project` folder:

```
Project\Tiremo\Generation\AUDK32_A34xxxx-1.0.12\Example\Source\TmplUserApp\config\mqtt_device_config.h
```

Find the device name definition:

```c
#define MQTT_DEVICE_NAME            "hun1"
```

Replace the `1` with **your own participant number**. For example, if you are participant **5**, set:

```c
#define MQTT_DEVICE_NAME            "hun5"
```


| Application            | Device name required?            |
| ---------------------- | -------------------------------- |
| **1 — Sensor Process** | No — MQTT is not used            |
| **2 — ESP32 MQTT**     | **Yes** — set before build/flash |
| **3 — SLM320 4G MQTT** | **Yes** — set before build/flash |


Save the file, then proceed to the application section below.

> **Note:** Pre-built HEX files in the `Binary` folder use the default name `hun1`. For MQTT applications, edit `mqtt_device_config.h` with your participant number and **build from the IDE** so your device name is included in the firmware.

---

### 3.6 Applications

The firmware includes **three workshop applications**. You select the active application in `app_config.h` by enabling **one** feature flag and keeping the other two commented out.

**Configuration file:**

```
..\Tiremo\Generation\AUDK32_A34xxxx-1.0.12\Example\Source\TmplUserApp\config\app_config.h
```

Relative to the extracted `Project` folder:

```
Project\Tiremo\Generation\AUDK32_A34xxxx-1.0.12\Example\Source\TmplUserApp\config\app_config.h
```


| Application            | Define                | Pre-built HEX file        |
| ---------------------- | --------------------- | ------------------------- |
| **1 — Sensor Process** | `EMPA_SENSOR_PROCESS` | `empa_sensor_process.hex` |
| **2 — ESP32 MQTT**     | `EMPA_ESP32_MQTT_AWS` | `empa_esp32_mqtt_aws.hex` |
| **3 — SLM320 4G MQTT** | `EMPA_SLM320_4G`      | `empa_slm320_4g.hex`      |


All HEX files are in the repository `Binary` folder:

```
..\Tiremo-Accelerator-Workshops-Secure-IoT\Binary
```

> **Important:** Only **one** of the three defines should be active at a time. Comment out the other two before building.

---

#### Application 1 — Sensor Process (`EMPA_SENSOR_PROCESS`)

This is the **default workshop application**. `EMPA_SENSOR_PROCESS` is already enabled in `app_config.h` — you do **not** need to change the define to run this application first.

```c
#define EMPA_SENSOR_PROCESS
//#define EMPA_ESP32_MQTT_AWS
//#define EMPA_SLM320_4G
```

**How it works**

- The board reads all onboard sensors (temperature, humidity, accelerometer, microphone, battery).
- Sensor values are printed to the **debug UART** (serial terminal).
- Press the **user button** once to **start** the measurement cycle; press again to **stop** it.
- While the cycle is stopped, the firmware waits idle and does not read sensors.

**Build and flash (IDE)**

1. Connect a **USB Type-C** cable to the **CN6** connector on your Tiremo®Cortex board.
2. Right-click the project in **Project Explorer** → **Build Project**.
3. Right-click the project again → **Run As → Run Configurations…**
4. Expand **GDB OpenOCD Debugging** and select **A34G43x**.
5. Click **Run** to flash the firmware.

Click Build

Click Run

Open a serial terminal (see [Section 5](#5-view-debug-messages-tera-term)) and press the button to start the sensor cycle.

**Flash via pre-built binary (optional)**

If you prefer not to build from source, load the pre-built HEX file using **aFlasher32**:

```
Binary\empa_sensor_process.hex
```

See [Section 4 — Flash via Binary (aFlasher32)](#4-flash-via-binary-aflasher32) for the full procedure.

---

#### Application 2 — ESP32 MQTT (`EMPA_ESP32_MQTT_AWS`)

This application connects to the Tiremo MQTT broker over **WiFi** using the onboard **ESP32-C3** module and publishes sensor data as JSON.

**Enable the application**

In `app_config.h`, uncomment `EMPA_ESP32_MQTT_AWS` and comment out the other two defines:

```c
//#define EMPA_SENSOR_PROCESS
#define EMPA_ESP32_MQTT_AWS
//#define EMPA_SLM320_4G
```

Make sure you completed [Section 3.5 — Configure Device Name](#35-configure-device-name) before building or flashing.

**How it works**

- The ABOV MCU reads sensors and sends JSON telemetry to the ESP32 over UART.
- The ESP32 connects to WiFi and publishes to the MQTT broker (`iot.tiremo.ai`) over TLS.
- On first boot, TLS certificates embedded in the ABOV flash are uploaded to the ESP32.
- After upload, the terminal prompts you to **hold the user button for 3 seconds** to erase the certificates from ABOV flash.
- Once erased, even a board reset will **not** trigger certificate upload again — the ESP32 uses its own stored certificates for MQTT.

**Build and flash (IDE)**

When switching from another application, terminate any active debug session first:

1. In the **Console / Terminal** view, right-click and select **Terminate / Disconnect All**.

Terminate / Disconnect All

1. Save `app_config.h` and `mqtt_device_config.h`.
2. Right-click the project → **Build Project**.
3. Right-click the project → **Run As → Run Configurations…** → **GDB OpenOCD Debugging → A34G43x** → **Run**.

**After flashing**

1. Open a serial terminal (see [Section 5](#5-view-debug-messages-tera-term)).
2. Wait for certificate upload and MQTT connection logs.
3. When prompted (`[CERT] Press and hold the button for 3+ seconds to erase the certificates stored in ABOV flash.`), **hold the button for 3 seconds** to erase ABOV flash certificates.
4. Sensor data is published automatically every 2 seconds.

**Flash via pre-built binary (optional)**

For MQTT applications, build from the IDE after setting your device name (see [Section 3.5](#35-configure-device-name)). To load the default pre-built HEX instead:

```
Binary\empa_esp32_mqtt_aws.hex
```

See [Section 4 — Flash via Binary (aFlasher32)](#4-flash-via-binary-aflasher32).

---

#### Application 3 — SLM320 4G MQTT (`EMPA_SLM320_4G`)

This application connects to the Tiremo MQTT broker over **4G LTE** using the **MeiG SLM320** modem and publishes sensor data as JSON. The workflow is the same as Application 2, but connectivity goes through the cellular module instead of WiFi.

**Enable the application**

In `app_config.h`, uncomment `EMPA_SLM320_4G` and comment out the other two defines:

```c
//#define EMPA_SENSOR_PROCESS
//#define EMPA_ESP32_MQTT_AWS
#define EMPA_SLM320_4G
```

Make sure you completed [Section 3.5 — Configure Device Name](#35-configure-device-name) before building or flashing.

**How it works**

- The ABOV MCU reads sensors and sends JSON telemetry to the SLM320 over UART.
- The SLM320 registers on the cellular network and connects to the MQTT broker over TLS.
- On first boot, TLS certificates embedded in the ABOV flash are uploaded to the SLM320.
- After upload, the terminal prompts you to **hold the user button for 3 seconds** to erase the certificates from ABOV flash.
- Once erased, even a board reset will **not** trigger certificate upload again — the SLM320 uses its own stored certificates for MQTT.

**Build and flash (IDE)**

1. In the **Console / Terminal** view, right-click and select **Terminate / Disconnect All**.

Terminate / Disconnect All

1. Save `app_config.h` and `mqtt_device_config.h`.
2. Right-click the project → **Build Project**.
3. Right-click the project → **Run As → Run Configurations…** → **GDB OpenOCD Debugging → A34G43x** → **Run**.

**After flashing**

1. Open a serial terminal (see [Section 5](#5-view-debug-messages-tera-term)).
2. Wait for the SLM320 network registration, certificate upload, and MQTT connection logs.
3. When prompted, **hold the button for 3 seconds** to erase ABOV flash certificates.
4. Sensor data is published automatically every 2 seconds.

**Flash via pre-built binary (optional)**

For MQTT applications, build from the IDE after setting your device name (see [Section 3.5](#35-configure-device-name)). To load the default pre-built HEX instead:

```
Binary\empa_slm320_4g.hex
```

See [Section 4 — Flash via Binary (aFlasher32)](#4-flash-via-binary-aflasher32).

---

## 4. Flash via Binary (aFlasher32)

Instead of flashing from the IDE, you can load a pre-built **HEX** file directly using **aFlasher32**.

### Step 1 — Locate the HEX file

In the downloaded repository, open the `Binary` folder:

```
..\Tiremo-Accelerator-Workshops-Secure-IoT\Binary
```

Select the HEX file for your application:


| Application            | HEX file                  |
| ---------------------- | ------------------------- |
| **1 — Sensor Process** | `empa_sensor_process.hex` |
| **2 — ESP32 MQTT**     | `empa_esp32_mqtt_aws.hex` |
| **3 — SLM320 4G MQTT** | `empa_slm320_4g.hex`      |


> For Application 2 and 3, complete [Section 3.5 — Configure Device Name](#35-configure-device-name) and build from the IDE if you need a unique participant number. Pre-built HEX files use the default `hun1` device name.

Binary folder — HEX file

### Step 2 — Open aFlasher32

Launch the **aFlasher32** application.

Click App

### Step 3 — Verify board connection

When the board is connected, a blue **aLinkUart2** message appears on screen — this confirms the connection.

aLinkUart2 connection message

### Step 4 — Select target device

In the **Target Device** field, select your board's microcontroller model.

Select target device

### Step 5 — Load the HEX file

Click **Code, Data, Cfg** and select the HEX file from the `Binary` folder.

Select HEX file via Code, Data, Cfg

### Step 6 — Clear read protection

A **Read Protection** dialog appears. Keep the default option **Clear read protection** selected and click **OK**.

Read Protection — Clear read protection

### Step 7 — Program the firmware

Click **Program** to flash the firmware onto the board.

Click Program

### Step 8 — Confirm success

When an **OK** message is displayed, the firmware has been loaded successfully.

Programming complete — OK message

---

## 5. View Debug Messages (Tera Term)

To monitor debug output from the board, use your preferred serial terminal or **Tera Term** as shown below.

### Step 1 — Open Tera Term

Launch the **Tera Term** application.

Open Tera Term

### Step 2 — Create a new connection

Go to **File → New connection**.

File → New connection

### Step 3 — Select the COM port

In the dialog, select the **COM port** assigned to your Tiremo®Cortex board.

Select COM port

### Step 4 — Configure serial port settings

After connecting, go to **Setup → Serial port** in the toolbar. Configure the UART settings, then click **New setting** to open the terminal.

Serial port settings — Setup

Serial port settings — New setting

You can now watch debug messages from the firmware in real time.

---

## 6. Configure Peripherals with MCUBrew32 (Optional)

> **Workshop note:** You do **not** need MCUBrew32 to complete the workshop activities. The repository already includes a fully generated AUDK32 project. Use this section only if you want to change pin, clock, or peripheral settings and regenerate the base project code.

**MCUBrew32** is ABOV's configuration tool for pin multiplexing, clock trees, and peripheral setup. It integrates with eMStudio32 and generates AUDK32-based starter code.

**Prerequisites:** [MCUBrew32 installed](../SetUp.md#2-mcubrew32-installation) and the project imported in eMStudio32 (see [Section 3](#3-open-the-project-in-emstudio32)).

### Step 1 — Open MCUBrew32 from eMStudio32

1. In eMStudio32, select the project in **Project Explorer**.
2. Click the **ABOV** button in the toolbar.s
3. Select **Config eMStudio32 Project**.

Open Config eMStudio32 Project from the ABOV menu

### Step 2 — Confirm the configuration dialogslm

In the dialog that opens, click **Finish**. MCUBrew32 launches with the current project settings.

Click Finish to open MCUBrew32

### Step 3 — Install the AUDK32 SDK (LOCAL)

Before generating code, MCUBrew32 needs the **AUDK32** package for the A34G43x family. The workshop repository includes the SDK archive:

```
Abov_SDK\AUDK32_A34xxxx-1.0.12.zip
```

> Keep the file as a `**.zip` archive** — do not extract it before installation.

1. In MCUBrew32, open the **PROJECT** menu on the toolbar.
2. Click **LOCAL**.
3. Browse to `Abov_SDK\AUDK32_A34xxxx-1.0.12.zip` in the cloned repository and select it.

PROJECT → LOCAL in MCUBrew32

Select AUDK32_A34xxxx-1.0.12.zip

1. Click **Install**.

Click Install

1. When **Complete Install** appears, click **OK**. The SDK is ready.

SDK installation complete

### Step 4 — Install the AUDK32 SDK (ONLINE, alternative)

Instead of a local ZIP file, you can download the same SDK version online:

1. In MCUBrew32, open **PROJECT → Manage**.

PROJECT → Manage

1. Select **AUDK32_A34xxxx-1.0.12** (same version as in the repository).
2. Click **Install** and wait for the download to finish.

Select AUDK32_A34xxxx-1.0.12 and click Install

### Step 5 — Configure peripherals and generate code

1. In MCUBrew32, set the device to **A34xxxx → A34G43x**.
2. Adjust pin, clock, and peripheral settings in the configuration panels.
3. Click **Code Generate** to export the updated project files.

Configure peripherals and click Code Generate

1. Return to eMStudio32 and run **Project → Clean…**, then **Project → Build Project**.

> **Warning:** Regenerating code may overwrite generated HAL and configuration files. Back up any custom changes under `Example/Source/TmplUserApp/` before generating.

### MCUBrew32 troubleshooting


| Error                                                           | Likely cause                          | Fix                                                                                                                      |
| --------------------------------------------------------------- | ------------------------------------- | ------------------------------------------------------------------------------------------------------------------------ |
| `Failed to preparation application : appBuilder.prepareBuilder` | AUDK32 SDK not installed in MCUBrew32 | Complete [Step 3](#step-3--install-the-audk32-sdk-local) or [Step 4](#step-4--install-the-audk32-sdk-online-alternative) |
| Device variant not available                                    | Wrong SDK version                     | Install **AUDK32_A34xxxx-1.0.12** exactly                                                                                |
| Generate overwrites custom code                                 | Expected MCUBrew32 behavior           | Back up `TmplUserApp` sources before generating                                                                          |


---

## Next Steps

- Review the firmware source and MQTT configuration in [Tiremo/README.md](Tiremo/README.md).
- Return to the [main workshop README](../README.md) for additional activities.

---

© Empa Electronics — Tiremo® Accelerator Workshops
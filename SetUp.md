# Development Environment Setup

This guide covers the installation of all software tools required to participate in the **Tiremo® Accelerator Workshops** activities. Complete every installation below before starting the workshop.

---

## Requirements

| Tool | Description |
|------|-------------|
| **eMStudio32** | Integrated development environment (IDE) for ABOV microcontrollers |
| **MCUBrew32** | Project creation and configuration tool |
| **aFlasher32** | Flash programming tool for loading binary files onto the board |
| **Tera Term** | Terminal emulator for serial port communication |

---

## 1. eMStudio32 Installation

eMStudio32 is an integrated development environment (IDE) for ABOV microcontrollers. It lets you write, build, and debug code from a single interface.

### Resources

- 🌐 **Official Download Page:** [ABOV Tools & Support — eMStudio32](https://www.abov.co.kr/en/tools_support/debug_tools.php?category=eMStudio32)
- 📖 **User Manual:** [eMStudio32 Manual & Release Notes](https://abov.atlassian.net/wiki/spaces/ES2/pages/1558413356/Manual+Release)
- 📋 **Installation Steps:** [eMStudio32 Installation Guide](https://abov.atlassian.net/wiki/spaces/ES2/pages/1752858627/Installation)
- 📄 **Local Document:** [ES2 Installation (PDF)](Document/ES2-Installation-200526-125944.pdf)

### Installation Steps

1. Go to the official download page above and download the installer.
2. Run the downloaded installer.
3. Follow the steps in the [installation documentation](https://abov.atlassian.net/wiki/spaces/ES2/pages/1752858627/Installation) to complete setup.

---

## 2. MCUBrew32 Installation

MCUBrew32 is a tool for code generation and peripheral configuration in ABOV microcontroller projects.

### Resources

- 🌐 **Official Download Page:** [ABOV Tools & Support — MCUBrew32](https://www.abov.co.kr/en/tools_support/debug_tools.php?category=mcubrew32)
- 📖 **User Manual:** [MCUBrew32 User Guide](https://abov.atlassian.net/wiki/spaces/MCUBrew321/pages/760250452/Manual+Release)
- 📋 **Installation Steps:** [MCUBrew32 Installation and Getting Started](https://abov.atlassian.net/wiki/spaces/MCUBrew321/pages/1379565598/Installation+and+Getting+Started)
- 📄 **Local Document:** [MCUBrew32 Installation (PDF)](Document/MCUBrew321-Installing%20and%20uninstalling%20the%20MCUBrew32%20program-200526-123146.pdf)

### Installation Steps

1. Go to the official download page above and download the installer.
2. Run the downloaded installer and follow the setup steps.
3. Installation is complete when the **"Installation Complete"** screen appears at **step 6** of the installation guide.

---

## 3. aFlasher32 Installation

aFlasher32 is a flash programming tool used to load compiled binary files onto the **Tiremo®Cortex** board.

### Installation Steps

**Step 1 —** Go to [ABOV Tools & Support — aFlasher32](https://www.abov.co.kr/en/tools_support/debug_tools.php?category=aflasher32).

![aFlasher32 download page](Document/aFlasher32/aFlasher_image1.png)

**Step 2 —** Under *All Tools & Support (Downloadable)*, download the **aFlasher32 Executable** `.exe` file from the **Executable File** column.

**Step 3 —** Extract the `.exe` installer from the downloaded `.zip` archive and run the setup program.

**Step 4 —** Complete the installation by following these steps:

**→** Click **Next** on the setup screen.

![aFlasher32 installation step 1](Document/aFlasher32/aFlasher_image2.png)

**→** Accept the license agreement.

![aFlasher32 license agreement](Document/aFlasher32/aFlasher_image3.png)

**→** Choose the installation location and click **Install**.

![aFlasher32 installation location](Document/aFlasher32/aFlasher_image4.png)

**→** Click **Finish** to complete the installation.

![aFlasher32 installation complete](Document/aFlasher32/aFlasher_image5.png)

---

## 4. Tera Term Installation

Tera Term is a terminal emulator used to communicate with **Tiremo®Cortex** over a serial port. This workshop uses Tera Term; you may use any serial terminal application you prefer.

### Installation Steps

**Step 1 —** Go to [https://teratermproject.github.io/index-en.html](https://teratermproject.github.io/index-en.html).

**Step 2 —** Click the latest release under **Download**.

![Tera Term download page](Document/TeraTerm/TeraTerm_image1.png)

**Step 3 —** On the release page, download the installer from the **installer** section.

![Tera Term installer](Document/TeraTerm/TeraTerm_image2.png)

**Step 4 —** Run the downloaded `.exe` file and complete the installation.

---

## Setup Complete ✓

Once all tools are installed, proceed to the workshop activity.

### ↳ [Start Activity — Running the Project](Project/RunningCode.md)

---

<p align="center">
  <sub>© Empa Electronics — Tiremo® Accelerator Workshops</sub>
</p>

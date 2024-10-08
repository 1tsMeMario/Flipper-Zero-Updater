# Flipper Zero Firmware Updater

Flipper Zero Firmware Updater is a C++ application designed to simplify the process of building or flashing firmware to your Flipper Zero device. The program provides a user-friendly interface to manage different firmware versions and flash them to your device via USB.

## Features

- **Download Firmware**: Easily download various firmware versions (Stock, Momentum, Xtreme, RogueMaster) from their respective GitHub repositories.
- **Build Firmware**: Compile the firmware for your Flipper Zero device using the `fbt` tool.
- **Flash Firmware**: Flash the compiled firmware directly to your Flipper Zero device via USB.
- **User-Friendly Interface**: Navigate through the options using a simple text-based menu.

## Prerequisites

Before using this application, ensure you have the following installed:

- **Git**: Required for cloning firmware repositories.
- **Windows**: This application is intended to be run on Windows operating systems.

## Usage

1. Place this application in a folder anywhere on your computer.
2. Run the application and select `Y` when it asks if you would like to download firmware:
   - It will download the latest versions of:
     • [Stock Firmware](https://github.com/flipperdevices/flipperzero-firmware)
     • [Momentum Firmware](https://github.com/Next-Flip/Momentum-Firmware)
     • [Xtreme Firmware](https://github.com/Flipper-XFW/Xtreme-Firmware)
     • [RogueMaster Firmware](https://github.com/RogueMaster/flipperzero-firmware-wPlugins)

3. Rerun the application once it finishes downloading.
4. Select the firmware you want to use.
5. Choose whether to build or flash the firmware:
   - Built firmware will be output to a folder called `FlipperZero-Update-Packages`. You can flash this using qFlipper.
   - If you decide to flash directly, the application will search for your Flipper Zero device.
   - Once found, it will begin building and flashing automatically.
   - If your Flipper device cannot be found:
     - Ensure your USB cable supports data transfer.
     - If it does, please try installing the serial port drivers provided in the most recent release.
       - [x64 version](https://github.com/1tsMeMario/Flipper-Zero-Updater/releases/download/v20240901104916/SerialPort_Driver_Installer_x64.exe)
       - [x32 version](https://github.com/1tsMeMario/Flipper-Zero-Updater/releases/download/v20240901104916/SerialPort_Driver_Installer_x32.exe)

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <setupapi.h>
#include <initguid.h>
#include <devguid.h>

#pragma comment(lib, "setupapi.lib")

namespace fs = std::filesystem;

void setColor(const std::string& color_code) {
    std::cout << color_code;
}

void resetColor() {
    std::cout << "\033[0m";
}

void executeCommand(const std::string& command) {
    system(command.c_str());
}

std::string getUserInput(const std::string& prompt) {
    std::string input;
    setColor("\033[1;33m");
    std::cout << prompt;
    resetColor();
    std::getline(std::cin, input);
    return input;
}

void git_clone(std::string url, std::string firmware_name)
{
    executeCommand("git clone --recursive --jobs 8 " + url + " " + firmware_name + " >nul 2>&1");
}
void downloadFirmware() {
    system("cls");

    setColor("\033[1;36m");
    std::cout << "Downloading Stock Firmware" << std::endl;
    resetColor();
    git_clone("https://github.com/flipperdevices/flipperzero-firmware.git", "Stock");
    std::cout << "Stock Firmware Downloaded" << std::endl;

    Sleep(2500);
    std::cout << "" << std::endl;

    setColor("\033[1;36m");
    std::cout << "Downloading Mementum Firmware" << std::endl;
    resetColor();
    git_clone("https://github.com/Next-Flip/Momentum-Firmware.git", "Momentum");
    std::cout << "Momentum Firmware Downloaded" << std::endl;


    Sleep(2500);
    std::cout << "" << std::endl;

    setColor("\033[1;36m");
    std::cout << "Downloading Xtreme Firmware" << std::endl;
    resetColor();
    git_clone("https://github.com/Flipper-XFW/Xtreme-Firmware.git", "Xtreme");
    std::cout << "Extreme Firmware Downloaded" << std::endl;

    Sleep(2500);
    std::cout << "" << std::endl;

    setColor("\033[1;36m");
    std::cout << "Downloading RogueMaster Firmware" << std::endl;
    resetColor();
    git_clone("https://github.com/RogueMaster/flipperzero-firmware-wPlugins.git", "RogueMaster");
    std::cout << "RogueMaster Firmware Downloaded" << std::endl;

    Sleep(2500);
    std::cout << "" << std::endl;

    setColor("\033[1;32m");
    std::cout << "All Firmware downloaded successfully.\n";
    resetColor();
    Sleep(2500);

    std::cout << "Please restart app" << std::endl;
    Sleep(2500);
    exit(0);
}

void no_firmware() {
    setColor("\033[1;31m");
    std::cout << "No firmware found.\n";
    resetColor();

    std::cout << "" << std::endl;

    std::string response = getUserInput("Would you like to download firmware? (Y/N): ");
    if (response == "Y" || response == "y") {
        downloadFirmware();
    }
    else {
        setColor("\033[1;36m");
        std::cout << "Exiting...\n";
        resetColor();
        Sleep(5000);
        exit(0);
    }
}

void listDirectories(const std::string& root, std::vector<std::string>& directories) {
    int count = 0;
    for (const auto& entry : fs::directory_iterator(root)) {
        if (entry.is_directory() && entry.path().filename() != "FlipperZero-Update-Packages") {
            directories.push_back(entry.path().filename().string());
            setColor("\033[1;34m");
            std::cout << "[" << ++count << "] " << entry.path().filename().string() << "\n";
            resetColor();
        }
    }

    if (count == 0) {
        no_firmware();
    }
    else {
        std::cout << "\n[0] Exit\n";
    }
}

std::vector<std::string> getComPortsWithFlipper() {
    std::vector<std::string> comPorts;

    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i;

    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT | DIGCF_PROFILE);

    if (hDevInfo == INVALID_HANDLE_VALUE) {
        return comPorts;
    }

    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++) {
        DWORD DataT;
        char buffer[256];
        DWORD buffersize = 256;

        if (SetupDiGetDeviceInstanceIdA(hDevInfo, &DeviceInfoData, buffer, buffersize, &buffersize)) {
            std::string deviceId(buffer);

            HKEY hDeviceRegistryKey = SetupDiOpenDevRegKey(hDevInfo, &DeviceInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
            if (hDeviceRegistryKey) {
                char portName[256];
                DWORD portNameSize = sizeof(portName);
                if (RegQueryValueExA(hDeviceRegistryKey, "PortName", NULL, NULL, (LPBYTE)portName, &portNameSize) == ERROR_SUCCESS) {
                    char friendlyName[256];
                    DWORD friendlyNameSize = sizeof(friendlyName);

                    if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, &DataT, (PBYTE)friendlyName, friendlyNameSize, &buffersize)) {
                        std::string friendlyNameStr(friendlyName);

                        if (friendlyNameStr.find("Flipper") != std::string::npos || friendlyNameStr.find("STMicroelectronics") != std::string::npos) {
                            comPorts.push_back(portName);
                        }
                    }
                }
                RegCloseKey(hDeviceRegistryKey);
            }
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    return comPorts;
}

void killProcess(const std::string& processName) {
    std::string command = "taskkill /f /im " + processName + " >nul 2>&1";
    system(command.c_str());
}

std::string findTgzFile(const std::string& path) {
    for (const auto& entry : fs::recursive_directory_iterator(path)) {
        if (entry.path().extension() == ".tgz") {
            return entry.path().string();
        }
    }
    return "";
}

void moveAndRenameTgzFile(const std::string& tgz_file, const std::string& firmware_name, const std::string& root) {
    if (!tgz_file.empty()) {
        std::string new_name = firmware_name + ".tgz";
        fs::path new_path = fs::path(root) / "FlipperZero-Update-Packages" / new_name;

        if (!fs::exists(fs::path(root) / "FlipperZero-Update-Packages")) {
            fs::create_directory(fs::path(root) / "FlipperZero-Update-Packages");
        }

        fs::rename(tgz_file, new_path);
        setColor("\033[1;32m");
        std::cout << firmware_name << " firmware update package built and moved successfully.\n";
        resetColor();
    }
    else {
        setColor("\033[1;31m");
        std::cout << "No .tgz file found in any folder under " << root << "\\" << firmware_name << "\\dist.\n";
        resetColor();
        Sleep(5000);
    }
}

void cleanUp(const std::string& path) {
    fs::remove_all(fs::path(path) / "build");
    fs::remove_all(fs::path(path) / "dist");
    Sleep(5000);
}

void buildFirmware(const std::string& root, const std::string& firmware_name) {
    std::string path = root + "\\" + firmware_name;
    if (!fs::exists(path)) {
        SetConsoleTitle("Failed to Find Folder");
        setColor("\033[1;31m");
        std::cout << "Failed to find " << firmware_name << " folder. Please check the name and try again.\n";
        resetColor();
        Sleep(5000);
        return;
    }

    SetConsoleTitle(("Building " + firmware_name + " Firmware").c_str());
    setColor("\033[1;36m");
    std::cout << "Building " << firmware_name << " firmware\n";
    resetColor();
    executeCommand("cd " + path + " && fbt");

    SetConsoleTitle(("Building " + firmware_name + " Firmware Update Package").c_str());
    std::cout << "Building " << firmware_name << " firmware update package\n";
    executeCommand("cd " + path + " && fbt updater_package");

    std::string tgz_file = findTgzFile(path + "\\dist");
    moveAndRenameTgzFile(tgz_file, firmware_name, root);

    SetConsoleTitle("Cleaning Up");
    cleanUp(path);
}

void flashFirmware(const std::string& root, const std::string& firmware_name) {

    SetConsoleTitle("Searching for Flipper Device");
    setColor("\033[1;36m");
    std::cout << "Searching for Flipper Device" << std::endl;
    resetColor();
    Sleep(2500);

    std::vector<std::string> flipperPorts = getComPortsWithFlipper();

    if (!flipperPorts.empty())
    {
        setColor("\033[1;32m");
        std::cout << "Detected your Flipper Zero on " << flipperPorts.front() << std::endl;
        resetColor();
        Sleep(2500);
        std::cout << "" << std::endl;

        std::string path = root + "\\" + firmware_name;
        if (!fs::exists(path)) {
            SetConsoleTitle("Failed to Find Folder");
            setColor("\033[1;31m");
            std::cout << "Failed to find " << firmware_name << " folder. Please check the name and try again.\n";
            resetColor();
            Sleep(5000);
            return;
        }

        SetConsoleTitle("Closing qFlipper");
        killProcess("qFlipper.exe");

        SetConsoleTitle(("Flashing " + firmware_name + " Firmware").c_str());
        setColor("\033[1;36m");
        std::cout << "Flashing " << firmware_name << " firmware via USB...\n";
        resetColor();
        executeCommand("cd " + path + " && fbt flash_usb_full");

        SetConsoleTitle("Firmware Flash Completed");
        setColor("\033[1;32m");
        std::cout << firmware_name << " firmware flashed successfully.\n";
        resetColor();
        Sleep(5000);
    }
    else {
        setColor("\033[1;31m");
        std::cout << "No Flipper device detected on a serial com port. Please connect your flipper and try again." << std::endl;
        resetColor();
        Sleep(2500);
        exit(0);
    }
}

int main() {
    SetConsoleTitle("Flipper Zero Updater");
    system("cls");
    std::cout << "Flipper Firmware Updater built by Brandon Mathew" << std::endl;
    Sleep(2000);
    std::string root = fs::current_path().string();
    std::vector<std::string> directories;
    std::string choice;

    while (true) {
        system("cls");
        SetConsoleTitle("Flipper Zero Updater");
        setColor("\033[1;34m");
        std::cout << "Firmware Selection Menu\n";
        resetColor();

        listDirectories(root, directories);
        choice = getUserInput("Please select a firmware: ");

        if (choice == "0") {
            SetConsoleTitle("Exiting");
            setColor("\033[1;36m");
            std::cout << "Exiting...\n";
            resetColor();
            Sleep(5000);
            break;
        }

        int index = std::stoi(choice) - 1;
        if (index < 0 || index >= directories.size()) {
            SetConsoleTitle("Invalid Selection");
            setColor("\033[1;31m");
            std::cout << "Invalid selection. Please try again.\n";
            resetColor();
            Sleep(5000);
            continue;
        }

        std::string firmware_name = directories[index];
        std::string action = getUserInput("Do you want to [B]uild or [F]lash the firmware? (B/F): ");

        if (action == "B" || action == "b") {
            buildFirmware(root, firmware_name);
        }
        else if (action == "F" || action == "f") {
            flashFirmware(root, firmware_name);
        }
        else {
            SetConsoleTitle("Invalid Selection");
            setColor("\033[1;31m");
            std::cout << "Invalid selection. Please try again.\n";
            resetColor();
            Sleep(5000);
        }
    }

    return 0;
}

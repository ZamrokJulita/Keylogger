#include <basetyps.h>
#include <stdlib.h>
#include <wtypes.h>
#include <initguid.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <ntddkbd.h>

#pragma warning(disable:4201)

#include <setupapi.h>
#include <winioctl.h>

#pragma warning(default:4201)

#include "..\sys\public.h"

//-----------------------------------------------------------------------------
// 4127 -- Conditional Expression is Constant warning
//-----------------------------------------------------------------------------
#define WHILE(constant) \
__pragma(warning(disable: 4127)) while(constant); __pragma(warning(default: 4127))

DEFINE_GUID(GUID_DEVINTERFACE_KBFILTER,
    0x3fb7299d, 0x6847, 0x4490, 0xb0, 0xc9, 0x99, 0xe0, 0x98, 0x6a, 0xb8, 0x86);
// {3FB7299D-6847-4490-B0C9-99E0986AB886}

#ifndef MAP_SC_TO_VK
#define MAP_SC_TO_VK 0x0
#endif

const char* makeCodeToText(USHORT makeCode) {
    switch (makeCode) {
    case 0x01: return "ESC";
    case 0x02: return "1";
    case 0x03: return "2";
    case 0x04: return "3";
    case 0x05: return "4";
    case 0x06: return "5";
    case 0x07: return "6";
    case 0x08: return "7";
    case 0x09: return "8";
    case 0x0A: return "9";
    case 0x0B: return "0";
    case 0x0C: return "-";
    case 0x0D: return "=";
    case 0x0E: return "BACKSPACE";
    case 0x0F: return "TAB";
    case 0x10: return "Q";
    case 0x11: return "W";
    case 0x12: return "E";
    case 0x13: return "R";
    case 0x14: return "T";
    case 0x15: return "Y";
    case 0x16: return "U";
    case 0x17: return "I";
    case 0x18: return "O";
    case 0x19: return "P";
    case 0x1A: return "[";
    case 0x1B: return "]";
    case 0x1C: return "ENTER";
    case 0x1D: return "CTRL";
    case 0x1E: return "A";
    case 0x1F: return "S";
    case 0x20: return "D";
    case 0x21: return "F";
    case 0x22: return "G";
    case 0x23: return "H";
    case 0x24: return "J";
    case 0x25: return "K";
    case 0x26: return "L";
    case 0x27: return ";";
    case 0x28: return "'";
    case 0x29: return "`";
    case 0x2A: return "SHIFT";
    case 0x2B: return "\\";
    case 0x2C: return "Z";
    case 0x2D: return "X";
    case 0x2E: return "C";
    case 0x2F: return "V";
    case 0x30: return "B";
    case 0x31: return "N";
    case 0x32: return "M";
    case 0x33: return ",";
    case 0x34: return ".";
    case 0x35: return "/";
    case 0x36: return "RIGHT SHIFT";
    case 0x38: return "ALT";
    case 0x39: return "SPACE";
    case 0x3A: return "CAPS LOCK";
    case 0x3B: return "F1";
    case 0x3C: return "F2";
    case 0x3D: return "F3";
    case 0x3E: return "F4";
    case 0x3F: return "F5";
    case 0x40: return "F6";
    case 0x41: return "F7";
    case 0x42: return "F8";
    case 0x43: return "F9";
    case 0x44: return "F10";
    case 0x57: return "F11";
    case 0x58: return "F12";

    default: return "Unknown Key";
    }
}

void logKeyToFile(const char* keyText) {
    FILE* file;
    errno_t err = fopen_s(&file, "keys_log.txt", "a");
    if (err != 0 || file == NULL) {
        printf("B³¹d przy otwieraniu pliku do zapisu.\n");
        return;
    }
    fprintf(file, "Klawisz: %s\n", keyText);
    fclose(file);
}

void DisplayKey(USHORT makeCode) {
    const char* keyText = makeCodeToText(makeCode);
    printf("Klawisz: %s (makeCode: 0x%02X)\n", keyText, makeCode);
    logKeyToFile(keyText);
}

int
_cdecl
main(
    _In_ int argc,
    _In_ char* argv[]
)
{
    HDEVINFO                            hardwareDeviceInfo;
    SP_DEVICE_INTERFACE_DATA            deviceInterfaceData;
    PSP_DEVICE_INTERFACE_DETAIL_DATA    deviceInterfaceDetailData = NULL;
    ULONG                               predictedLength = 0;
    ULONG                               requiredLength = 0, bytes = 0;
    HANDLE                              file;
    ULONG                               i = 0;
    USHORT                              makeCode = 0;
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);

    hardwareDeviceInfo = SetupDiGetClassDevs(
        (LPGUID)&GUID_DEVINTERFACE_KBFILTER,
        NULL, // Define no enumerator (global)
        NULL, // Define no
        (DIGCF_PRESENT | // Only Devices present
            DIGCF_DEVICEINTERFACE)); // Function class devices.
    if (INVALID_HANDLE_VALUE == hardwareDeviceInfo)
    {
        printf("SetupDiGetClassDevs failed: %x\n", GetLastError());
        return 0;
    }

    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    printf("\nList of KBFILTER Device Interfaces\n");
    printf("---------------------------------\n");

    i = 0;

    do {
        if (SetupDiEnumDeviceInterfaces(hardwareDeviceInfo,
            0, // No care about specific PDOs
            (LPGUID)&GUID_DEVINTERFACE_KBFILTER,
            i, //
            &deviceInterfaceData)) {

            if (deviceInterfaceDetailData) {
                free(deviceInterfaceDetailData);
                deviceInterfaceDetailData = NULL;
            }

            if (!SetupDiGetDeviceInterfaceDetail(
                hardwareDeviceInfo,
                &deviceInterfaceData,
                NULL,  // probing so no output buffer yet
                0,     // probing so output buffer length of zero
                &requiredLength,
                NULL)) {
                if (ERROR_INSUFFICIENT_BUFFER != GetLastError()) {
                    printf("SetupDiGetDeviceInterfaceDetail failed %d\n", GetLastError());
                    SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
                    return 0;
                }
            }

            predictedLength = requiredLength;
            deviceInterfaceDetailData = malloc(predictedLength);

            if (deviceInterfaceDetailData) {
                deviceInterfaceDetailData->cbSize =
                    sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            }
            else {
                printf("Couldn't allocate %d bytes for device interface details.\n", predictedLength);
                SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
                return 0;
            }

            if (!SetupDiGetDeviceInterfaceDetail(
                hardwareDeviceInfo,
                &deviceInterfaceData,
                deviceInterfaceDetailData,
                predictedLength,
                &requiredLength,
                NULL)) {
                printf("Error in SetupDiGetDeviceInterfaceDetail\n");
                SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);
                free(deviceInterfaceDetailData);
                return 0;
            }
            printf("%d) %s\n", ++i, deviceInterfaceDetailData->DevicePath);
        }
        else if (ERROR_NO_MORE_ITEMS != GetLastError()) {
            free(deviceInterfaceDetailData);
            deviceInterfaceDetailData = NULL;
            continue;
        }
        else
            break;

    } WHILE(TRUE);

    SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

    if (!deviceInterfaceDetailData)
    {
        printf("No device interfaces present\n");
        return 0;
    }

    printf("\nOpening the last interface:\n %s\n", deviceInterfaceDetailData->DevicePath);

    file = CreateFile(deviceInterfaceDetailData->DevicePath,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL, // no SECURITY_ATTRIBUTES structure
        OPEN_EXISTING, // No special create flags
        0, // No special attributes
        NULL);

    if (INVALID_HANDLE_VALUE == file) {
        printf("Error in CreateFile: %x", GetLastError());
        free(deviceInterfaceDetailData);
        return 0;
    }
    while (1) {
        if (!DeviceIoControl(file,
            IOCTL_KBFILTR_GET_KEYBOARD_ATTRIBUTES,
            NULL, 0,
            &makeCode, sizeof(makeCode),
            &bytes, NULL)) {
            printf("Retrieve Keyboard Attributes request failed:0x%x\n", GetLastError());
            return 0;
        }
        if (makeCode != 0x00) {
            DisplayKey(makeCode);
        }

        if (makeCode == 0x01) {
            printf("Exiting...\n");
            break;
        }

        Sleep(100);
    }

    free(deviceInterfaceDetailData);
    CloseHandle(file);
}

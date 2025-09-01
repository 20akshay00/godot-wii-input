// Graciously adapted from WiiPair project - https://github.com/jordanbtucker/WiiPair
#ifdef _WIN32
#include <windows.h>
#include <bthsdpdef.h>
#include <bthdef.h>
#include <BluetoothAPIs.h>
#include <tchar.h>
#include <strsafe.h>
#include <godot_cpp/variant/utility_functions.hpp>
#include <string>
#include <sstream>

#pragma comment(lib, "Bthprops.lib")

// Wrapper for Godot printing with "{}" substitution
inline void gd_print(const std::string &fmt)
{
    godot::UtilityFunctions::print(fmt.c_str());
}

template <typename T, typename... Args>
void gd_print(const std::string &fmt, T value, Args... args)
{
    std::ostringstream oss;
    size_t pos = fmt.find("{}");
    if (pos != std::string::npos)
    {
        oss << fmt.substr(0, pos) << value << fmt.substr(pos + 2);
        gd_print(oss.str(), args...);
    }
    else
    {
        // no placeholder, append value at the end
        oss << fmt << " " << value;
        gd_print(oss.str(), args...);
    }
}

DWORD ShowErrorCode(const char *msg, DWORD dw)
{
    char *lpMsgBuf;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&lpMsgBuf, 0, NULL);

    // Trim trailing newlines
    for (char *p = lpMsgBuf + strlen(lpMsgBuf) - 1; p >= lpMsgBuf && (*p == '\n' || *p == '\r'); --p)
        *p = 0;

    gd_print("{}: {}", msg, lpMsgBuf);
    LocalFree(lpMsgBuf);
    return dw;
}

char *FormatBTAddress(BLUETOOTH_ADDRESS address)
{
    static char ret[20];
    sprintf_s(ret, "%02x:%02x:%02x:%02x:%02x:%02x",
              address.rgBytes[5],
              address.rgBytes[4],
              address.rgBytes[3],
              address.rgBytes[2],
              address.rgBytes[1],
              address.rgBytes[0]);
    return ret;
}

void pair_wiimotes()
{
    HANDLE hRadios[256];
    int nRadios;
    int nPaired = 0;

    ///////////////////////////////////////////////////////////////////////
    // Enumerate BT radios
    ///////////////////////////////////////////////////////////////////////
    {
        HBLUETOOTH_RADIO_FIND hFindRadio;
        BLUETOOTH_FIND_RADIO_PARAMS radioParam;

        // gd_print("Enumerating radios...");

        radioParam.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);

        nRadios = 0;
        hFindRadio = BluetoothFindFirstRadio(&radioParam, &hRadios[nRadios++]);
        if (hFindRadio)
        {
            while (BluetoothFindNextRadio(&radioParam, &hRadios[nRadios++]))
                ;
            BluetoothFindRadioClose(hFindRadio);
        }
        else
        {
            ShowErrorCode("Error enumerating radios", GetLastError());
            return;
        }
        nRadios--;
        // gd_print("Found {} radios", nRadios);
    }

    ///////////////////////////////////////////////////////////////////////
    // Keep looping until we pair with a Wii device
    ///////////////////////////////////////////////////////////////////////

    while (nPaired == 0)
    {
        int radio;

        for (radio = 0; radio < nRadios; radio++)
        {
            BLUETOOTH_RADIO_INFO radioInfo;
            HBLUETOOTH_DEVICE_FIND hFind;
            BLUETOOTH_DEVICE_INFO btdi;
            BLUETOOTH_DEVICE_SEARCH_PARAMS srch;

            radioInfo.dwSize = sizeof(radioInfo);
            btdi.dwSize = sizeof(btdi);
            srch.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);

            // ShowErrorCode("BluetoothGetRadioInfo", BluetoothGetRadioInfo(hRadios[radio], &radioInfo));

            // gd_print("Radio {}: {} {}", radio, std::wstring(radioInfo.szName).c_str(), FormatBTAddress(radioInfo.address));

            srch.fReturnAuthenticated = TRUE;
            srch.fReturnRemembered = TRUE;
            srch.fReturnConnected = TRUE;
            srch.fReturnUnknown = TRUE;
            srch.fIssueInquiry = TRUE;
            srch.cTimeoutMultiplier = 2;
            srch.hRadio = hRadios[radio];

            gd_print("Scanning...");

            hFind = BluetoothFindFirstDevice(&srch, &btdi);

            if (hFind == NULL)
            {
                if (GetLastError() == ERROR_NO_MORE_ITEMS)
                {
                    gd_print("No bluetooth devices found.");
                }
                else
                {
                    ShowErrorCode("Error enumerating devices", GetLastError());
                    return;
                }
            }
            else
            {
                do
                {
                    // gd_print("Found: {}: {}", std::wstring(btdi.szName).c_str(), FormatBTAddress(btdi.Address));

                    if (!wcscmp(btdi.szName, L"Nintendo RVL-WBC-01") || !wcscmp(btdi.szName, L"Nintendo RVL-CNT-01") || !wcscmp(btdi.szName, L"Nintendo RVL-CNT-01-TR"))
                    {
                        WCHAR pass[6];
                        DWORD pcServices = 16;
                        GUID guids[16];
                        BOOL error = FALSE;

                        if (!error)
                        {
                            if (btdi.fRemembered)
                            {
                                // Make Windows forget pairing
                                if (ShowErrorCode("BluetoothRemoveDevice", BluetoothRemoveDevice(&btdi.Address)) != ERROR_SUCCESS)
                                    error = TRUE;
                            }
                        }

                        // MAC address is passphrase
                        pass[0] = btdi.Address.rgBytes[0];
                        pass[1] = btdi.Address.rgBytes[1];
                        pass[2] = btdi.Address.rgBytes[2];
                        pass[3] = btdi.Address.rgBytes[3];
                        pass[4] = btdi.Address.rgBytes[4];
                        pass[5] = btdi.Address.rgBytes[5];

                        if (!error)
                        {
                            // Pair with Wii device
                            if (ShowErrorCode("BluetoothAuthenticateDevice", BluetoothAuthenticateDevice(NULL, hRadios[radio], &btdi, pass, 6)) != ERROR_SUCCESS)
                                error = TRUE;
                        }

                        if (!error)
                        {
                            // If this is not done, the Wii device will not remember the pairing
                            if (ShowErrorCode("BluetoothEnumerateInstalledServices", BluetoothEnumerateInstalledServices(hRadios[radio], &btdi, &pcServices, guids)) != ERROR_SUCCESS)
                                error = TRUE;
                        }

                        if (!error)
                        {
                            // Activate service
                            if (ShowErrorCode("BluetoothSetServiceState", BluetoothSetServiceState(hRadios[radio], &btdi, &HumanInterfaceDeviceServiceClass_UUID, BLUETOOTH_SERVICE_ENABLE)) != ERROR_SUCCESS)
                                error = TRUE;
                        }

                        if (!error)
                        {
                            nPaired++;
                        }
                    }
                } while (BluetoothFindNextDevice(hFind, &btdi));
            }
        }

        Sleep(1000);
    }

    ///////////////////////////////////////////////////////////////////////
    // Clean up
    ///////////////////////////////////////////////////////////////////////

    {
        int radio;

        for (radio = 0; radio < nRadios; radio++)
        {
            CloseHandle(hRadios[radio]);
        }
    }

    gd_print("Wiimotes paired: {}", nPaired);

    return;
}

#else

void pair_wiimotes()
{
    // Not necessary on non-Windows platforms
    return;
}

#endif

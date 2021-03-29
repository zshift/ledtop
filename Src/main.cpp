#define _WIN32_DCOM
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "../Include/LogitechLEDLib.h"

using namespace std;

#include <comdef.h>
#include <Wbemidl.h>

#include "asus.cpp"
#include "openhardwaremonitor.cpp"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "LogitechLEDLib.lib")
#pragma comment(lib, "advapi32.lib")

bool setup();
bool logiSetLed(uint64_t color);
void shutdown();
uint64_t getColor(temp t, uint64_t limit);

int main()
{
    if (!setup())
    {
        shutdown();
        return 1;
    }

    AsusLed *asus = AsusLed::Init();
    if (asus == nullptr)
    {
        cout << "Failed to initialize Asus led." << endl;
        return 1;
    }

    OpenHardwareMonitor *ohm = OpenHardwareMonitor::Init();
    if (ohm == nullptr)
    {
        cout << "Failed to initialize OpenHardwareMonitor." << endl;
        return 1;
    }

    while(true)
    {
        temps t = ohm->GetTemps();
        if (t.nil == true)
        {
            cout << "Failed to get temps." << endl;
        }

        uint64_t cpuColor = getColor(t.cpu, 90);
        uint64_t gpuColor = getColor(t.gpu, 85);

        logiSetLed(gpuColor);
        asus->SetLeds(cpuColor);

        cout << "GPU temp: " << t.gpu.val << "C" << endl;
        cout << "CPU temp: " << t.cpu.val << "C" << endl << endl;

        this_thread::sleep_for(chrono::milliseconds(1000));
    }

    delete asus;
    delete ohm;
    shutdown();
    return 0;
}

bool logiSetup();
bool comSetup();

bool setup()
{
    return logiSetup() && comSetup();
}

bool logiSetup()
{
    if (!LogiLedInit())
    {
        cout << "Init was not successfull" << endl;
        return false;
    }
    if (!LogiLedSaveCurrentLighting())
    {
        cout << "Could not save current lighting" << endl;
        LogiLedShutdown();
        return false;
    }
    return true;
}

bool comSetup()
{
    HRESULT hr;
    hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        cout << "Failed to initialize COM library. Error code = 0x" << hex << hr << endl;
        return false;
    }

    hr = CoInitializeSecurity(
        NULL,
        -1,                          // COM authentication
        NULL,                        // Authentication services
        NULL,                        // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
        RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
        NULL,                        // Authentication info
        EOAC_NONE,                   // Additional capabilities
        NULL                         // Reserved
    );

    if (FAILED(hr))
    {
        cout << "Failed to initialize security. Error code 0x" << hex << hr << endl;
        CoUninitialize();
        return false;
    }
    return true;
}

uint64_t getColor(temp t, uint64_t limit) {
    if (t.val >= limit) {
        return 0xFF0000FF; // RED
    }

    if (t.val >= limit * 0.9) {
        return 0xFF0064FF; // Orange at 90%
    }

    if (t.val >= limit * 0.8) {
        return 0xFF00FFFF; // Yellow at 80%
    }

    if (t.val >= limit * 0.7) {
        return 0xFF00FF64; // YellowGreen at 70%
    }

    return 0xFF00FF00; // Green below 80%
}

bool logiSetLed(uint64_t color)
{
    int redPct = (color & 0xFF) * 100 / 255;
    int greenPct = ((color & 0xFF00) >> 8) * 100 / 255;
    int bluePct = ((color & 0xFF0000) >> 16) * 100 / 255;

    if (!LogiLedSetLighting(redPct, greenPct, bluePct))
    {
        cout << "Could not set light color" << endl;
        return false;
    }

    return true;
}

void shutdown()
{
    LogiLedRestoreLighting();
    LogiLedShutdown();

    CoUninitialize();
}
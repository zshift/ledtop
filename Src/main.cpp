#define _WIN32_DCOM
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <comdef.h>
#include <Wbemidl.h>

#include "asus.h"
#include "openhardwaremonitor.h"
#include "../Include/LogitechLEDLib.h"

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "LogitechLEDLib.lib")
#pragma comment(lib, "advapi32.lib")

bool setup();
bool logiSetLed(uint64_t color);
void shutdown();
uint64_t getColor(temp t, uint64_t limit);
void handle_eptr(std::exception_ptr eptr);

int main()
{
    if (!setup())
    {
        shutdown();
        return 1;
    }

    auto asus = AsusLed::Init();
    if (asus == nullptr)
    {
        std::cout << "Failed to initialize Asus led." << std::endl;
        return 1;
    }

    auto ohm = OpenHardwareMonitor::Init();
    if (ohm == nullptr)
    {
        std::cout << "Failed to initialize OpenHardwareMonitor." << std::endl;
        return 1;
    }

    while (true)
    {
        temps t = ohm->GetTemps();
        if (t.nil == true)
        {
            std::cout << "Failed to get temps." << std::endl;
        }

        uint64_t cpuColor = getColor(t.cpu, 90);
        uint64_t gpuColor = getColor(t.gpu, 85);

        logiSetLed(gpuColor);
        asus->SetLeds(cpuColor);

        std::cout << "GPU temp: " << t.gpu.val << "C" << std::endl;
        std::cout << "CPU temp: " << t.cpu.val << "C" << std::endl
             << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

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
    std::exception_ptr eptr;
    try
    {

        if (!LogiLedInit())
        {
            std::cout << "Unable to connect to Logitech devices. Make sure you are running Logitech Gaming Software." << std::endl;
            return false;
        }
        if (!LogiLedSaveCurrentLighting())
        {
            std::cout << "Could not save current Logi lighting" << std::endl;
            LogiLedShutdown();
            return false;
        }
    }
    catch (...)
    {
        eptr = std::current_exception();
        handle_eptr(eptr);
        return false;
    }
    return true;
}

bool comSetup()
{
    HRESULT hr;
    hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        std::cout << "Failed to initialize COM library. Error code = 0x" << std::hex << hr << std::endl;
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
        std::cout << "Failed to initialize security. Error code 0x" << std::hex << hr << std::endl;
        CoUninitialize();
        return false;
    }
    return true;
}

// Returns a color in ABGR hex.
uint64_t getColor(temp t, uint64_t limit)
{
    if (t.val >= limit) return 0xFF0000FF; // RED
    if (t.val >= limit * 0.9) return 0xFF0064FF; // Orange at 90%
    if (t.val >= limit * 0.8) return 0xFF00FFFF; // Yellow at 80
    if (t.val >= limit * 0.7) return 0xFF00FF64; // YellowGreen at 70%
    return 0xFF00FF00; // Green below 80%
}

bool logiSetLed(uint64_t color)
{
    auto redPct = (color & 0xFF) * 100 / 255;
    auto greenPct = ((color & 0xFF00) >> 8) * 100 / 255;
    auto bluePct = ((color & 0xFF0000) >> 16) * 100 / 255;

    if (!LogiLedSetLighting(redPct, greenPct, bluePct))
    {
        std::cout << "Could not set light color" << std::endl;
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

void handle_eptr(std::exception_ptr eptr)
{
    try
    {
        if (eptr)
        {
            std::rethrow_exception(eptr);
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Caught exception \"" << e.what() << "\"\n";
    }
}
#include <cstdint>
#include <Wbemidl.h>
#include <memory>

#pragma once

struct temp
{
    uint32_t min;
    uint32_t max;
    uint32_t val;
};

struct temps
{
    bool nil;
    temp gpu;
    temp cpu;
};

class OpenHardwareMonitor
{
public:
    OpenHardwareMonitor(IWbemLocator *pLocator, IWbemServices *pServices)
        : pLoc(pLocator),
          pSvc(pServices) {}
    ~OpenHardwareMonitor();

    static std::unique_ptr<OpenHardwareMonitor> Init();
    temps GetTemps();

private:
    IWbemLocator *pLoc;
    IWbemServices *pSvc;
};
#include <cstdint>
#include <Wbemidl.h>
#include <memory>

using namespace std;

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
private:
    IWbemLocator *pLoc;
    IWbemServices *pSvc;

public:
    OpenHardwareMonitor(IWbemLocator *pLocator, IWbemServices *pServices)
        : pLoc(pLocator),
          pSvc(pServices) {}
    static unique_ptr<OpenHardwareMonitor> Init();
    temps GetTemps();
    ~OpenHardwareMonitor();
};
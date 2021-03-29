#include <memory>
#import "libid:F1AA5209-5217-4B82-BA7E-A68198999AFA"

using namespace std;
using namespace AuraServiceLib;

#pragma once

class AsusLed
{
private:
    IAuraSdkPtr sdk;
    IAuraSyncDeviceCollectionPtr devices;

public:
    AsusLed(IAuraSdkPtr sdkPtr, IAuraSyncDeviceCollectionPtr devicesPtr)
        : sdk(sdkPtr),
          devices(devicesPtr) {}
    static unique_ptr<AsusLed> Init();
    void SetLeds(uint64_t color);
    ~AsusLed();
};
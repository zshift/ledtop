#include <memory>
#import "libid:F1AA5209-5217-4B82-BA7E-A68198999AFA"

#pragma once

class AsusLed
{
public:
    AsusLed(AuraServiceLib::IAuraSdkPtr sdkPtr, AuraServiceLib::IAuraSyncDeviceCollectionPtr devicesPtr)
        : sdk(sdkPtr),
          devices(devicesPtr) {}
    ~AsusLed();

    static std::unique_ptr<AsusLed> Init();
    void SetLeds(uint64_t color);

private:
    AuraServiceLib::IAuraSdkPtr sdk;
    AuraServiceLib::IAuraSyncDeviceCollectionPtr devices;
};
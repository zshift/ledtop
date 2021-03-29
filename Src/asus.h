#include <memory>
#import "libid:F1AA5209-5217-4B82-BA7E-A68198999AFA"

#pragma once

using namespace std;
using namespace AuraServiceLib;

class AsusLed
{
private:
    IAuraSdkPtr sdk;
    IAuraSyncDeviceCollectionPtr devices;
    AsusLed(IAuraSdkPtr sdkPtr, IAuraSyncDeviceCollectionPtr devicesPtr);
public:
    static unique_ptr<AsusLed> Init();
    void SetLeds(uint64_t color);
    ~AsusLed();
};
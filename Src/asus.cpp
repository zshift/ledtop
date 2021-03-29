#include <iostream>
#import "libid:F1AA5209-5217-4B82-BA7E-A68198999AFA"

using namespace std;
using namespace AuraServiceLib;

#pragma once

class AsusLed
{
private:
    IAuraSdkPtr sdk;
    IAuraSyncDeviceCollectionPtr devices;

    AsusLed()
    {
        sdk = nullptr;
        devices = nullptr;
    }

    AsusLed(IAuraSdkPtr sdkPtr, IAuraSyncDeviceCollectionPtr devicesPtr)
    {
        sdk = sdkPtr;
        devices = devicesPtr;
    }

public:
    static AsusLed *Init()
    {
        // CreateAura  SDK Instance
        IAuraSdkPtr sdk = nullptr;
        HRESULT hr = sdk.CreateInstance(__uuidof(AuraSdk), nullptr, CLSCTX_INPROC_SERVER);
        if (FAILED(hr))
        {
            cout << "Failed to create Aura sdk instance. Error code 0x" << hex << hr << endl;
            CoUninitialize();
            return nullptr;
        }

        // Aqcuire Control
        sdk->SwitchMode();

        // Enumerate all devices
        IAuraSyncDeviceCollectionPtr devices;

        devices = sdk->Enumerate(0); // 0 means ALL

        return new AsusLed(sdk, devices);
    }

    void SetLeds(uint64_t color)
    {
        // Traverse all devices
        for (int i = 0; i < devices->Count; i++)
        {
            IAuraSyncDevicePtr dev = devices->Item[i];
            IAuraRgbLightCollectionPtr lights = dev->Lights;
            for (int j = 0; j < lights->Count; j++)
            {
                IAuraRgbLightPtr light = lights->Item[j];
                light->Color = color;
            }
            dev->Apply();
        }
    }

    ~AsusLed()
    {
        sdk->Release();
        devices->Release();
    }
};
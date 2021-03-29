#include <iostream>
#include "asus.h"

class AsusLed
{
private:
    IAuraSdkPtr sdk;
    IAuraSyncDeviceCollectionPtr devices;

    AsusLed(IAuraSdkPtr sdkPtr, IAuraSyncDeviceCollectionPtr devicesPtr)
    {
        sdk = sdkPtr;
        devices = devicesPtr;
    }

public:
    static unique_ptr<AsusLed> Init()
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
        auto devices = sdk->Enumerate(0); // 0 means ALL

        return make_unique<AsusLed>(sdk, devices);
    }

    void SetLeds(uint64_t color)
    {
        // Traverse all devices
        for (int i = 0; i < devices->Count; i++)
        {
            auto dev = devices->Item[i];
            auto lights = dev->Lights;
            for (int j = 0; j < lights->Count; j++)
            {
                auto light = lights->Item[j];
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
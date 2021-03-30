#include <iostream>

#include "asus.h"

AsusLed::~AsusLed()
{
    sdk->Release();
    devices->Release();
}

std::unique_ptr<AsusLed> AsusLed::Init()
{
    using namespace AuraServiceLib;

    // CreateAura  SDK Instance
    IAuraSdkPtr sdk = nullptr;
    HRESULT hr = sdk.CreateInstance(__uuidof(AuraSdk), nullptr, CLSCTX_INPROC_SERVER);
    if (FAILED(hr))
    {
        std::cout << "Failed to create Aura sdk instance. Error code 0x" << std::hex << hr << std::endl;
        CoUninitialize();
        return nullptr;
    }

    // Aqcuire Control
    sdk->SwitchMode();

    // Enumerate all devices
    const int ALL = 0;
    auto devices = sdk->Enumerate(ALL);

    return std::make_unique<AsusLed>(sdk, devices);
}

void AsusLed::SetLeds(uint64_t color)
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
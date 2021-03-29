#include <iostream>
#include <comdef.h>
#include <Wbemidl.h>

using namespace std;

#pragma once

struct temp {
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

    OpenHardwareMonitor()
    {
        pLoc = nullptr;
        pSvc = nullptr;
    }

    OpenHardwareMonitor(IWbemLocator *pLocator, IWbemServices *pServices)
    {
        pLoc = pLocator;
        pSvc = pServices;
    }

public:
    static OpenHardwareMonitor *Init()
    {
        IWbemLocator *pLoc = nullptr;

        HRESULT hr = CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            (LPVOID *)&pLoc);

        if (FAILED(hr))
        {
            cout << "Failed to create IWbemLocator object."
                 << " Err code = 0x" << hex << hr << endl;
            CoUninitialize();
            return nullptr;
        }

        IWbemServices *pSvc = nullptr;

        // Connect to the root\cimv2 namespace with
        // the current user and obtain pointer pSvc
        // to make IWbemServices calls.
        hr = pLoc->ConnectServer(
            _bstr_t("root\\OpenHardwareMonitor"), // Object path of WMI namespace
            NULL,                                 // User name. NULL = current user
            NULL,                                 // User password. NULL = current
            0,                                    // Locale. NULL indicates current
            NULL,                                 // Security flags.
            0,                                    // Authority (for example, Kerberos)
            0,                                    // Context object
            &pSvc                                 // pointer to IWbemServices proxy
        );
        if (FAILED(hr))
        {
            cout << "Could not connect. Error code = 0x" << hex << hr << endl;
            pLoc->Release();
            return nullptr;
        }

        hr = CoSetProxyBlanket(
            pSvc,                        // Indicates the proxy to set
            RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
            RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
            NULL,                        // Server principal name
            RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
            RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
            NULL,                        // client identity
            EOAC_NONE                    // proxy capabilities
        );
        if (FAILED(hr))
        {
            cout << "Could not set proxy blanket. Error code = 0x" << hex << hr << endl;
            pSvc->Release();
            pLoc->Release();
            return nullptr;
        }

        return new OpenHardwareMonitor(pLoc, pSvc);
    }

    temps GetTemps()
    {
        temps t;

        IEnumWbemClassObject *pEnumerator = NULL;
        HRESULT hr = pSvc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Sensor"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &pEnumerator);
        if (FAILED(hr))
        {
            cout << "Query for Sensors name failed."
                 << " Error code = 0x"
                 << hex << hr << endl;
            t.nil = true;
            return t;
        }

        IWbemClassObject *pclsObj = NULL;
        ULONG uReturn = 0;

        while (pEnumerator)
        {
            hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
            if (FAILED(hr) || 0 == uReturn)
            {
                break;
            }

            VARIANT vtProp;
            hr = pclsObj->Get(L"SensorType", 0, &vtProp, NULL, NULL);
            if (FAILED(hr) || 0 != wcscmp(vtProp.bstrVal, L"Temperature"))
            {
                continue;
            }
            
            float min, max, val;

            hr = pclsObj->Get(L"Value", 0, &vtProp, NULL, NULL);
            if (FAILED(hr)) {
                cout << "Failed to get temp value." << endl;
                continue;
            }
            val = vtProp.fltVal;
            
            hr = pclsObj->Get(L"Min", 0, &vtProp, NULL, NULL);
            if (FAILED(hr)) {
                cout << "Failed to get temp value." << endl;
                continue;
            }
            min = vtProp.fltVal;
            
            hr = pclsObj->Get(L"Max", 0, &vtProp, NULL, NULL);
            if (FAILED(hr)) {
                cout << "Failed to get temp value." << endl;
                continue;
            }
            max = vtProp.fltVal;
            
            hr = pclsObj->Get(L"Name", 0, &vtProp, NULL, NULL);
            if (FAILED(hr)) {
                cout << "Failed to get temp value." << endl;
                continue;
            }
            if (0 == wcscmp(vtProp.bstrVal, L"GPU Core")) {
                t.gpu.val = val;
                t.gpu.min = min;
                t.gpu.max = max;
            }
            if (0 == wcscmp(vtProp.bstrVal, L"CPU Package")) {
                t.cpu.val = val;
                t.cpu.min = min;
                t.cpu.max = max;
            }

            // Debug, prints out everything.
            // BSTR objText = nullptr;
            // hr = pclsObj->GetObjectText(0, &objText);
            // wcout << objText << endl;

            VariantClear(&vtProp);

            pclsObj->Release();
        }

        pEnumerator->Release();

        return t;
    }

    ~OpenHardwareMonitor()
    {
        pSvc->Release();
        pLoc->Release();
    }
};
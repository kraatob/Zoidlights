#include "Device.h"

Device::Device() :
    m_device(nullptr),
	m_deviceContext(nullptr) {
    Init();
}

Device::~Device() {
    if (m_device)
    {
        m_device->Release();
        m_device = nullptr;
    }

	if (m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = nullptr;
    }
}

void Device::Init() {
    // Driver types supported
    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    // Feature levels supported
    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };
#pragma warning(suppress: 26812)
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr;
    // Create device
    for (UINT driverTypeIndex = 0; driverTypeIndex < ARRAYSIZE(driverTypes); ++driverTypeIndex) {
        hr = D3D11CreateDevice(nullptr, driverTypes[driverTypeIndex], nullptr, 0, featureLevels, ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &m_device, &featureLevel, &m_deviceContext);
        if (SUCCEEDED(hr)) {
            break;
        }
    }
    if (FAILED(hr)) {
        throw L"Device creation failed";
    }
}

IDXGIOutput1* Device::GetDxgiOutput1(UINT outputNumber) {
    // Get DXGI device
    IDXGIDevice* dxgiDevice = nullptr;
    HRESULT hr = m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (FAILED(hr)) {
        throw L"Failed to QI for DXGI Device";
    }

    // Get DXGI adapter
    IDXGIAdapter* dxgiAdapter = nullptr;
    hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
    dxgiDevice->Release();
    if (FAILED(hr)) {
        throw L"Failed to get parent DXGI Adapter";
    }

    // Get output
    IDXGIOutput* dxgiOutput = nullptr;
    hr = dxgiAdapter->EnumOutputs(outputNumber, &dxgiOutput);
    dxgiAdapter->Release();
    if (FAILED(hr)) {
        throw L"Failed to get specified output in DUPLICATIONMANAGER";
    }

    // QI for Output 1
    IDXGIOutput1* dxgiOutput1 = nullptr;
    hr = dxgiOutput->QueryInterface(__uuidof(dxgiOutput1), reinterpret_cast<void**>(&dxgiOutput1));
    dxgiOutput->Release();
    if (FAILED(hr)) {
        throw L"Failed to QI for DxgiOutput1 in DUPLICATIONMANAGER";
    }

    return dxgiOutput1;
}

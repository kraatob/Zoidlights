#include "DuplicationManager.h"

DuplicationManager::DuplicationManager() : m_DeskDupl(nullptr),
                                           m_AcquiredDesktopImage(nullptr),
                                           m_Output(0),
                                           m_Device(nullptr),
										   m_DeviceContext(nullptr)
{
    RtlZeroMemory(&m_OutputDesc, sizeof(m_OutputDesc));
}

DuplicationManager::~DuplicationManager()
{
	if (m_DeskDupl)
    {
        m_DeskDupl->Release();
        m_DeskDupl = nullptr;
    }

    if (m_AcquiredDesktopImage)
    {
        m_AcquiredDesktopImage->Release();
        m_AcquiredDesktopImage = nullptr;
    }

    if (m_Device)
    {
        m_Device->Release();
        m_Device = nullptr;
    }

	if (m_DeviceContext)
    {
        m_DeviceContext->Release();
        m_DeviceContext = nullptr;
    }
}

// 
// Init the desktop duplication for display number Output.
// 
void DuplicationManager::InitDuplication(UINT Output)
{
	m_Output = Output;

    // Driver types supported
    D3D_DRIVER_TYPE DriverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

    // Feature levels supported
    D3D_FEATURE_LEVEL FeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };
    UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);
    D3D_FEATURE_LEVEL FeatureLevel;

    HRESULT hr;
    // Create device
    for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
    {
        hr = D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, D3D11_CREATE_DEVICE_DEBUG, FeatureLevels, NumFeatureLevels,
        D3D11_SDK_VERSION, &m_Device, &FeatureLevel, &m_DeviceContext);
        if (SUCCEEDED(hr))
        {
            // Device creation succeeded, no need to loop anymore
            break;
        }
    }
    if (FAILED(hr))
    {
        throw "Device creation in OUTPUTMANAGER failed";
    }


  // Get DXGI device
    IDXGIDevice* DxgiDevice = nullptr;
    hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&DxgiDevice));
    if (FAILED(hr))
    {
        throw "Failed to QI for DXGI Device";
    }

    // Get DXGI adapter
    IDXGIAdapter* DxgiAdapter = nullptr;
    hr = DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&DxgiAdapter));
    DxgiDevice->Release();
    DxgiDevice = nullptr;
    if (FAILED(hr))
    {
        throw "Failed to get parent DXGI Adapter";
    }

    // Get output
    IDXGIOutput* DxgiOutput = nullptr;
    hr = DxgiAdapter->EnumOutputs(Output, &DxgiOutput);
    DxgiAdapter->Release();
    DxgiAdapter = nullptr;
    if (FAILED(hr))
    {
        throw "Failed to get specified output in DUPLICATIONMANAGER";
    }

    DxgiOutput->GetDesc(&m_OutputDesc);

    // QI for Output 1
    IDXGIOutput1* DxgiOutput1 = nullptr;
    hr = DxgiOutput->QueryInterface(__uuidof(DxgiOutput1), reinterpret_cast<void**>(&DxgiOutput1));
    DxgiOutput->Release();
    DxgiOutput = nullptr;
    if (FAILED(hr))
    {
        throw "Failed to QI for DxgiOutput1 in DUPLICATIONMANAGER";
    }

    // Create desktop duplication
    hr = DxgiOutput1->DuplicateOutput(m_Device, &m_DeskDupl);
    DxgiOutput1->Release();
    DxgiOutput1 = nullptr;
    if (FAILED(hr))
    {
        if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE)
        {
            throw "There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.";
        }
        throw "Failed to get duplicate output in DUPLICATIONMANAGER";
    }
}

void DuplicationManager::UpdateFrame()
{
    IDXGIResource* DesktopResource = nullptr;
    DXGI_OUTDUPL_FRAME_INFO FrameInfo;
    m_DeskDupl->ReleaseFrame();
    HRESULT hr = m_DeskDupl->AcquireNextFrame(50, &FrameInfo, &DesktopResource);

    if (hr == DXGI_ERROR_WAIT_TIMEOUT)
    {
		OutputDebugStringW(L"Timeout!");
        return;
    }

    // If still holding old frame, destroy it
    if (m_AcquiredDesktopImage)
    {
        m_AcquiredDesktopImage->Release();
        m_AcquiredDesktopImage = nullptr;
    }

    // QI for IDXGIResource
    hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&m_AcquiredDesktopImage));
    DesktopResource->Release();
    DesktopResource = nullptr;
    if (FAILED(hr))
    {
        throw "Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER";
    }
    OutputDebugStringW(L"Updated!.");
}

#include "DesktopDuplication.h"

DesktopDuplication::DesktopDuplication(Device *device) :
    m_dxgiOutputDuplication(nullptr),
	m_acquiredDesktopImage(nullptr) {
    m_device = device;
}

DesktopDuplication::~DesktopDuplication() {
    if (m_dxgiOutputDuplication) {
		m_dxgiOutputDuplication->ReleaseFrame();
        m_dxgiOutputDuplication->Release();
        m_dxgiOutputDuplication = nullptr;
    }

    if (m_acquiredDesktopImage) {
        m_acquiredDesktopImage->Release();
        m_acquiredDesktopImage = nullptr;
    }
}

// 
// Init the desktop duplication for display number outputNumber.
// 
void DesktopDuplication::StartDuplication(UINT outputNumber) {
    IDXGIOutput1* dxgiOutput1 = m_device->GetDxgiOutput1(outputNumber);

    // Create desktop duplication
    HRESULT hr = dxgiOutput1->DuplicateOutput(m_device->m_device, &m_dxgiOutputDuplication);
    dxgiOutput1->Release();
    if (FAILED(hr)) {
        if (hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE) {
            throw "There is already the maximum number of applications using the Desktop Duplication API running, please close one of those applications and then try again.";
        }
        throw "Failed to get duplicate output in DUPLICATIONMANAGER";
    }
}

void DesktopDuplication::UpdateFrame()
{
    IDXGIResource* DesktopResource = nullptr;
    DXGI_OUTDUPL_FRAME_INFO FrameInfo;

    // Release last frame right before acquiring new one as per recommendation
    m_dxgiOutputDuplication->ReleaseFrame();
    HRESULT hr = m_dxgiOutputDuplication->AcquireNextFrame(5, &FrameInfo, &DesktopResource);

    if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
        // Display has not been updated
        return;
    }

    // Release old frame
    if (m_acquiredDesktopImage) {
        m_acquiredDesktopImage->Release();
    }

    // QI for IDXGIResource
    hr = DesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&m_acquiredDesktopImage));
    DesktopResource->Release();
    if (FAILED(hr)) {
        throw "Failed to QI for ID3D11Texture2D from acquired IDXGIResource in DUPLICATIONMANAGER";
    }
}

#pragma once

#include "framework.h"
#include "Device.h"

struct Size {
	UINT cx;
	UINT cy;
};

class DesktopDuplication
{
	public:
		DesktopDuplication(Device *device);
		~DesktopDuplication();
		void StartDuplication(UINT outputNumber);
		void UpdateFrame();
		Size GetDimensions();
		ID3D11Texture2D* m_acquiredDesktopImage;

	private:
		Device *m_device;
		UINT m_outputNumber;
		IDXGIOutputDuplication* m_dxgiOutputDuplication;
};


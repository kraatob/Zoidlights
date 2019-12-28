#pragma once

#include "framework.h"
#include "Device.h"

class DesktopDuplication
{
	public:
		DesktopDuplication(Device *device);
		~DesktopDuplication();
		void StartDuplication(UINT outputNumber);
		void UpdateFrame();
		ID3D11Texture2D* m_acquiredDesktopImage;

	private:
		Device *m_device;
		IDXGIOutputDuplication* m_dxgiOutputDuplication;
};


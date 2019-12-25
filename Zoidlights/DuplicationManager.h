#pragma once

#include "framework.h"

class DuplicationManager
{
	public:
		DuplicationManager();
		~DuplicationManager();
		void InitDuplication(UINT Output);
		void UpdateFrame();
		ID3D11Texture2D* m_AcquiredDesktopImage;
		ID3D11DeviceContext* m_DeviceContext;
		ID3D11Device* m_Device;

	private:
		IDXGIOutputDuplication* m_DeskDupl;
		UINT m_Output;
		DXGI_OUTPUT_DESC m_OutputDesc;
};


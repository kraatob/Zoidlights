#include "DesktopDuplication.h"

#pragma once
class LightRegion
{
	public:
		LightRegion(Device* device, DesktopDuplication* desktopDuplication, UINT x, UINT y, UINT width, UINT height);
		~LightRegion();
		DWORD getColor();
	private:
		DesktopDuplication* m_desktopDuplication;
		Device* m_device;
		ID3D11Texture2D* m_texture;
		D3D11_BOX m_box;
		UINT m_width;
		UINT m_height;
		DWORD AverageColor(D3D11_MAPPED_SUBRESOURCE* mappedResource, UINT width, UINT height);
		void InitTexture();
};


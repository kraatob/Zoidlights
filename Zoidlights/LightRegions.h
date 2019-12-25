#include "DuplicationManager.h"

#pragma once
class LightRegions
{
	public:
		LightRegions(DuplicationManager* DuplicationManager);
		~LightRegions();
		void InitLightRegion();
		DWORD getColor();
	private:
		DuplicationManager* m_DuplicationManager;
		ID3D11Texture2D* m_Texture;
		D3D11_BOX m_Box;
		DWORD AverageColor(D3D11_MAPPED_SUBRESOURCE* MappedResource, UINT width, UINT height);
};


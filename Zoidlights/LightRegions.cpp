#include "LightRegions.h"

LightRegions::LightRegions(DuplicationManager* lDuplicationManager)
{
	m_DuplicationManager = lDuplicationManager;
}

LightRegions::~LightRegions()
{
	if (m_Texture) {
		m_Texture->Release();
	}
}

void LightRegions::InitLightRegion()
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = 10;
	desc.Height = 10;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	m_DuplicationManager->m_Device->CreateTexture2D(&desc, nullptr, &m_Texture);

	m_Box = D3D11_BOX{ 100, 100, 0, 110, 110, 1 };
}

DWORD LightRegions::getColor()
{
	m_DuplicationManager->UpdateFrame();

	D3D11_TEXTURE2D_DESC desc;
    m_DuplicationManager->m_AcquiredDesktopImage->GetDesc(&desc);
	// OPTIMIZE: see https://github.com/Microsoft/graphics-driver-samples/blob/master/render-only-sample/rostest/util.cpp#L244

	m_DuplicationManager->m_DeviceContext->CopySubresourceRegion(m_Texture, 0, 0, 0, 0, m_DuplicationManager->m_AcquiredDesktopImage, 0, &m_Box);

	D3D11_MAPPED_SUBRESOURCE MapInfo;

	HRESULT hr = m_DuplicationManager->m_DeviceContext->Map(m_Texture, 0, D3D11_MAP_READ, 0, &MapInfo);
	if (FAILED(hr)) {
		throw L"Failed to map staging texture";
	}
	DWORD color = AverageColor(&MapInfo, 10, 10);
	m_DuplicationManager->m_DeviceContext->Unmap(m_Texture, 0);
	return color;
}

DWORD LightRegions::AverageColor(D3D11_MAPPED_SUBRESOURCE* MappedResource, UINT width, UINT height)
{
	BYTE* pixel = (BYTE*)MappedResource->pData;
	UINT r = 0, g = 0, b = 0;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			b += *(pixel++);
			g += *(pixel++);
			r += *(pixel++);
			pixel++;
		}
		pixel += MappedResource->RowPitch - 4 * width;
	}
	UINT count = width * height;
	return RGB(r / count, g / count, b / count);
}

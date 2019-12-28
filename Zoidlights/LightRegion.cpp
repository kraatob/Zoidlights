#include "LightRegion.h"

LightRegion::LightRegion(DuplicationManager* l_duplicationManager, UINT x, UINT y, UINT width, UINT height)
{
	m_duplicationManager = l_duplicationManager;
	InitTexture();
	m_box = D3D11_BOX{ x, y, 0, x + width, y + height, 1 };
	m_width = width;
	m_height = height;
}

LightRegion::~LightRegion()
{
	if (m_texture) {
		m_texture->Release();
	}
}

void LightRegion::InitTexture()
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

	m_duplicationManager->m_Device->CreateTexture2D(&desc, nullptr, &m_texture);
}

DWORD LightRegion::getColor()
{
	D3D11_TEXTURE2D_DESC desc;
    m_duplicationManager->m_AcquiredDesktopImage->GetDesc(&desc);
	// OPTIMIZE: see https://github.com/Microsoft/graphics-driver-samples/blob/master/render-only-sample/rostest/util.cpp#L244

	m_duplicationManager->m_DeviceContext->CopySubresourceRegion(m_texture, 0, 0, 0, 0, m_duplicationManager->m_AcquiredDesktopImage, 0, &m_box);

	D3D11_MAPPED_SUBRESOURCE MapInfo;

	HRESULT hr = m_duplicationManager->m_DeviceContext->Map(m_texture, 0, D3D11_MAP_READ, 0, &MapInfo);
	if (FAILED(hr)) {
		throw L"Failed to map staging texture";
	}
	DWORD color = AverageColor(&MapInfo, m_width, m_height);
	m_duplicationManager->m_DeviceContext->Unmap(m_texture, 0);
	return color;
}

DWORD LightRegion::AverageColor(D3D11_MAPPED_SUBRESOURCE* MappedResource, UINT width, UINT height)
{
	BYTE* pixel = (BYTE*)MappedResource->pData;
	UINT r = 0, g = 0, b = 0;
	for (UINT y = 0; y < height; y++)
	{
		for (UINT x = 0; x < width; x++)
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

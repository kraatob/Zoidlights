#include "LightRegion.h"
#include "Constants.h"
#include <math.h>

#define SAMPLES_SQUARE (SAMPLES * SAMPLES)
#define HALF_SAMPLE (SAMPLES - 1) / 2
DWORD rSamples[SAMPLES], gSamples[SAMPLES], bSamples[SAMPLES];


LightRegion::LightRegion(Device* device, DesktopDuplication* desktopDuplication, UINT x, UINT y, UINT width, UINT height, BOOL vertical, UINT lightCount) {
	m_vertical = vertical;
	m_device = device;
	m_desktopDuplication = desktopDuplication;
	m_width = width;
	m_height = height;
	m_lightCount = lightCount;
	m_box = D3D11_BOX{ x, y, 0, x + width, y + height, 1 };
	m_lights = new COLORREF[lightCount];
	InitTexture();
}

LightRegion::~LightRegion()
{
	if (m_texture) {
		m_texture->Release();
	}
	delete[] m_lights;
}

void LightRegion::InitTexture()
{
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = m_width;
	desc.Height = m_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.SampleDesc = DXGI_SAMPLE_DESC{ 1, 0 };
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MiscFlags = 0;

	m_device->m_device->CreateTexture2D(&desc, nullptr, &m_texture);
}

void LightRegion::UpdateLights() {
	if (!m_desktopDuplication->m_acquiredDesktopImage) {
		return;
	}
	D3D11_TEXTURE2D_DESC desc;
    m_desktopDuplication->m_acquiredDesktopImage->GetDesc(&desc);
	// OPTIMIZE: see https://github.com/Microsoft/graphics-driver-samples/blob/master/render-only-sample/rostest/util.cpp#L244

	m_device->m_deviceContext->CopySubresourceRegion(m_texture, 0, 0, 0, 0, m_desktopDuplication->m_acquiredDesktopImage, 0, &m_box);

	D3D11_MAPPED_SUBRESOURCE MapInfo;

	HRESULT hr = m_device->m_deviceContext->Map(m_texture, 0, D3D11_MAP_READ, 0, &MapInfo);
	if (FAILED(hr)) {
		throw L"Failed to map staging texture";
	}
	CalculateLightColors(&MapInfo);
	m_device->m_deviceContext->Unmap(m_texture, 0);
}

void LightRegion::CalculateLightColors(D3D11_MAPPED_SUBRESOURCE* mappedResource) {
	BYTE* pixel = (BYTE*)mappedResource->pData;
	INT smallSkip;
    INT bigSkip;

	if (m_vertical) {
		smallSkip = 4 * ((m_width - 1) / (SAMPLES - 1)) - 2;
		bigSkip = ((m_height - 1) / (m_lightCount - 1)) * mappedResource->RowPitch - SAMPLES * (smallSkip + 2);
	} else {
		smallSkip = ((m_height - 1) / (SAMPLES - 1)) * mappedResource->RowPitch - 2;
		bigSkip = ((m_width - 1) / (m_lightCount - 1)) * 4 - SAMPLES * (smallSkip + 2);
	}
	INT resetSkip = -SAMPLES * (smallSkip + 2);

	COLORREF* output = m_lights - SAMPLES;
	UINT sampleCounter = 0;
	DWORD rSample = 0, gSample = 0, bSample = 0;
	rSamples[0] = gSamples[0] = bSamples[0] = 0;
	for (INT largeDirectionCounter = -HALF_SAMPLE; largeDirectionCounter < (INT)m_lightCount + HALF_SAMPLE; largeDirectionCounter++) {
		sampleCounter++;
		sampleCounter %= SAMPLES;
		for (INT smallDirectionCounter = 0; smallDirectionCounter < SAMPLES; smallDirectionCounter++) {
			bSample += *pixel * *pixel;
			pixel++;
			gSample += *pixel * *pixel;
			pixel++;
			rSample += *pixel * *pixel;
			pixel += smallSkip;
		}
		if (largeDirectionCounter >= 0 && largeDirectionCounter <(INT)m_lightCount - 1) {
			pixel += bigSkip;
		} else {
			pixel += resetSkip;
		}
		output++;
		DWORD oldR = rSamples[sampleCounter], oldG = gSamples[sampleCounter], oldB = bSamples[sampleCounter];
		rSamples[sampleCounter] = rSample; gSamples[sampleCounter] = gSample; bSamples[sampleCounter] = bSample;
		if (output >= m_lights) {
			*output = RGB(sqrt((rSample - oldR) / SAMPLES_SQUARE), sqrt((gSample - oldG) / SAMPLES_SQUARE), sqrt((bSample - oldB) / SAMPLES_SQUARE));
		}
	}
}

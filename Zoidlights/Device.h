#include "framework.h"

#pragma once
class Device {
public:
	Device();
	~Device();
	void Init();
	IDXGIOutput1* GetDxgiOutput1(UINT Output);
	ID3D11DeviceContext* m_deviceContext;
	ID3D11Device* m_device;
};


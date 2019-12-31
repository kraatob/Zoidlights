#include "framework.h"
#include "Device.h"
#include "DesktopDuplication.h"
#include "LightRegion.h"
#include "Light.h"

#pragma once
class LightManager {
public:
	LightManager();
	~LightManager();

	Light* m_lights;
	UINT m_lightCount;

	BOOL Update();

private:
	LightRegion* m_lightRegions[4];
	DesktopDuplication* m_desktopDuplication;
	Device* m_device;
	BOOL m_isSetup;

	void Setup();
	void InitLights();
	void Teardown();
};


#include "LightManager.h"
#include "Constants.h"

LightManager::LightManager() :
    m_isSetup(FALSE),
    m_lights(nullptr),
    m_lightCount(0),
    m_device(nullptr),
    m_desktopDuplication(nullptr) {
    RtlFillMemory(m_lightRegions, sizeof(m_lightRegions), 0);
}

LightManager::~LightManager() {
    if (m_device) {
        delete m_device;
        m_device = nullptr;
    }
    if (m_desktopDuplication) {
        delete m_desktopDuplication;
        m_desktopDuplication = nullptr;
    }
    if (m_lights) {
        delete[] m_lights;
        m_lights = nullptr;
    }
    for (int i = 0; i < 4; i++) {
        if (m_lightRegions[i]) {
            delete m_lightRegions[i];
        }
        m_lightRegions[i] = nullptr;
    }
}

void LightManager::Setup() {
    m_device = new Device();
    m_desktopDuplication = new DesktopDuplication(m_device);
    m_desktopDuplication->StartDuplication(1);
    InitLights();
    m_isSetup = TRUE;
}

void LightManager::InitLights() {
    Size dimensions = m_desktopDuplication->GetDimensions();
    double aspectRatio = (double)dimensions.cx / (double)dimensions.cy;
    UINT sampleDistance = (UINT)(BORDER_SIZE_OF_X * dimensions.cx - 1) / (SAMPLES - 1);
    UINT border = sampleDistance * (SAMPLES - 1) + 1;
    UINT lightCountX = dimensions.cx / sampleDistance;
    UINT lightCountY = dimensions.cy / sampleDistance;
    m_lightCount = 2 * (lightCountX + lightCountY);
    m_lights = new Light[m_lightCount];

    UINT lightCounter = 0;

    // top
    m_lightRegions[0] = new LightRegion(m_device, m_desktopDuplication, 0, 0, dimensions.cx, border, FALSE, lightCountX);
    for (UINT i = 0; i < lightCountX; i++) {
        Light *light = &m_lights[lightCounter++];
        light->color = &m_lightRegions[0]->m_lights[i];
        light->x = i * (1.0 / (lightCountX - 1));
        light->y = 0;
   }

    // right
    m_lightRegions[1] = new LightRegion(m_device, m_desktopDuplication, dimensions.cx - border, 0, border, dimensions.cy, TRUE, lightCountY);
    for (UINT i = 0; i < lightCountY; i++) {
        Light *light = &m_lights[lightCounter++];
        light->color = &m_lightRegions[1]->m_lights[i];
        light->x = 1; 
        light->y = i * (1.0 / (lightCountY - 1));
   }

    // bottom
    m_lightRegions[2] = new LightRegion(m_device, m_desktopDuplication, 0, dimensions.cy - border, dimensions.cx, border, FALSE, lightCountX);
    for (UINT i = 0; i < lightCountX; i++) {
        Light *light = &m_lights[lightCounter++];
        light->color = &m_lightRegions[2]->m_lights[i];
        light->x = i * (1.0 / (lightCountX - 1));
        light->y = 1;
   }

    // left
    m_lightRegions[3] = new LightRegion(m_device, m_desktopDuplication, 0, 0, border, dimensions.cy, TRUE, lightCountY);
    for (UINT i = 0; i < lightCountY; i++) {
        Light *light = &m_lights[lightCounter++];
        light->color = &m_lightRegions[3]->m_lights[i];
        light->x = 0;
        light->y = i * (1.0 / (lightCountY - 1));
   }
}

BOOL LightManager::Update() {
    try {
		if (!m_isSetup) {
			Setup();
		}
        m_desktopDuplication->UpdateFrame();
        for (int i = 0; i < 4; i++) {
            m_lightRegions[i]->UpdateLights();
        }
    } catch (const wchar_t* exception) {
        OutputDebugStringW(exception);
        return FALSE;
    }
    return TRUE;
}

#pragma once

#include "resource.h"
#include "Device.h"
#include "LightRegion.h"

struct Light {
	LightRegion *region;
	double x;
	double y;
	DWORD color;
};

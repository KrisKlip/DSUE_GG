#include "DroneControllerStates.h"

#include "Core.h"

static TAutoConsoleVariable<int32> CVarDroneControllerMode(
	TEXT("drone.controller.mode"),
	0,
	TEXT("Defines Controller modes.\n")
	TEXT("0 regular mode")
	TEXT("  1: Calibration mode"),
	ECVF_Scalability | ECVF_RenderThreadSafe);
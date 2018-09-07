#pragma once
 
#include "IDroneControllerModule.h"
 
DECLARE_LOG_CATEGORY_EXTERN(DroneControllerModule, All, All);

class FDroneControllerModule : public IInputDeviceModule
{
	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;
};

#pragma once
 
#include "IDroneControllerModule.h"

class FDroneController;
 
DECLARE_LOG_CATEGORY_EXTERN(DroneControllerModule, All, All);

class FDroneControllerModule : public IDroneControllerModule
{
public:

	virtual TSharedRef<FDroneController> GetDroneControllerRef() override { return DroneController.ToSharedRef(); }

private:
	virtual TSharedPtr< class IInputDevice > CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;

	
private:
	TSharedPtr<FDroneController> DroneController = nullptr;
};

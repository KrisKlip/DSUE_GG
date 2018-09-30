#include "DroneControllerModule.h"
#include "DroneController.h"
 
DEFINE_LOG_CATEGORY(DroneControllerModule);
 
#define LOCTEXT_NAMESPACE "FDroneControllerModule"

void FDroneControllerModule::StartupModule()
{
	IInputDeviceModule::StartupModule();
	FDroneController::PreInit();
}

TSharedPtr< class IInputDevice > FDroneControllerModule::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	DroneController = TSharedPtr< class FDroneController >(new FDroneController(InMessageHandler));
	return DroneController;
}

 
#undef LOCTEXT_NAMESPACE
 
IMPLEMENT_MODULE(FDroneControllerModule,DroneControllerModule)
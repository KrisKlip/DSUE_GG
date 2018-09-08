#include "DroneControllerModule.h"
#include "DroneController.h"
 
DEFINE_LOG_CATEGORY(DroneControllerModule);
 
#define LOCTEXT_NAMESPACE "FDroneControllerModule"

TSharedPtr< class IInputDevice > FDroneControllerModule::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	DroneController = TSharedPtr< class FDroneController >(new FDroneController(InMessageHandler));
	return DroneController;
}

 
#undef LOCTEXT_NAMESPACE
 
IMPLEMENT_MODULE(FDroneControllerModule,DroneControllerModule)
#include "DroneControllerModule.h"
#include "DroneController.h"
 
DEFINE_LOG_CATEGORY(DroneControllerModule);
 
#define LOCTEXT_NAMESPACE "FDroneControllerModule"

TSharedPtr< class IInputDevice > FDroneControllerModule::CreateInputDevice(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler)
{
	return TSharedPtr< class IInputDevice >(new FDroneController(InMessageHandler));
}
 
#undef LOCTEXT_NAMESPACE
 
IMPLEMENT_MODULE(FDroneControllerModule,DroneControllerModule)
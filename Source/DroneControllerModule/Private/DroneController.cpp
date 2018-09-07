#include "DroneController.h"

FDroneController::FDroneController(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	UE_LOG(LogTemp, Warning, TEXT("FDroneController::FDroneController"));
}

FDroneController::~FDroneController()
{
	UE_LOG(LogTemp, Warning, TEXT("FDroneController::~FDroneController"));
}

void FDroneController::Tick(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("FDroneController::Tick"));
}

void FDroneController::SendControllerEvents()
{
	//UE_LOG(LogTemp, Warning, TEXT("FDroneController::SendControllerEvents"));
}

void FDroneController::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	UE_LOG(LogTemp, Warning, TEXT("FDroneController::SetMessageHandler"));
}

void FDroneController::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	UE_LOG(LogTemp, Warning, TEXT("FDroneController::SetChannelValue"));
}

void FDroneController::SetChannelValues(int32 ControllerId, const FForceFeedbackValues & Values)
{
	UE_LOG(LogTemp, Warning, TEXT("FDroneController::SetChannelValues"));
}

bool FDroneController::Exec(UWorld * InWorld, const TCHAR * Cmd, FOutputDevice & Ar)
{
	return false;
}

bool FDroneController::IsGamepadAttached() const
{
	return true;
}

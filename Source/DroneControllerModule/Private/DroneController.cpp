#include "DroneController.h"
#include "HAL/PlatformTime.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"
#include "InputCoreTypes.h"
#include "Modules/ModuleManager.h"
#include "GenericPlatform/IInputInterface.h"
#include "Engine.h"
#include "HAL/PlatformProcess.h"

#include "IDroneControllerModule.h"
#include "DroneControllerModule.h"

#include "UE4x360ce.h"

#define LOCTEXT_NAMESPACE "DroneController"


namespace DroneControllerKeyNames
{
	const FGamepadKeyNames::Type Hook("DroneController_Hook");
	const FGamepadKeyNames::Type DroneController_Stick_Left_X("DroneController_Stick_Left_X");
}

namespace DroneControllerKeys
{
	const FKey DroneController_Stick_Left_X("DroneController_Stick_Left_X");
}


FDroneController::FDroneController(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
	: MessageHandler(InMessageHandler)
	, bIsSendControllerEvents(false)
	, DeltaTime(0.f)
	, ue4x360ce(nullptr)
{
	UE_LOG(DroneControllerModule, Warning, TEXT("FDroneController::FDroneController"));

#if PLATFORM_64BITS
	FString WinDir = TEXT("Win64/");
#else
	FString WinDir = TEXT("Win32/");
#endif

	FString RootXInputPath = FPaths::ProjectDir() / FString::Printf(TEXT("Source/DroneControllerModule/Binaries/")) / WinDir;

	FPlatformProcess::PushDllDirectory(*RootXInputPath);
	UE4x360ceHandle = FPlatformProcess::GetDllHandle(*(RootXInputPath + "UE4x360ce/UE4x360ce.dll"));
	FPlatformProcess::PopDllDirectory(*RootXInputPath);


	if (!UE4x360ceHandle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load UE4x360ce library."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Success to load UE4x360ce library."));
	}

	EKeys::AddMenuCategoryDisplayInfo("DroneController", LOCTEXT("DroneControllerSubCateogry", "DroneController"), TEXT("GraphEditor.PadEvent_16x"));

	// Add buttons
	Buttons[0][EDroneControllerButton::Hook] = DroneControllerKeyNames::Hook;

	// Add button keys
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::Hook, LOCTEXT("DroneController_Hook", "DroneController Hook"), FKeyDetails::GamepadKey, "DroneController"));

	// Float axis
	EKeys::AddKey(FKeyDetails(DroneControllerKeys::DroneController_Stick_Left_X, LOCTEXT("DroneController_Stick_Left_X", "DroneController Left Stick X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "DroneController"));

	ue4x360ce = new UE4x360ce();

	ue4x360ce->Run();
}

FDroneController::~FDroneController()
{
	if (UE4x360ceHandle)
	{
		FPlatformProcess::FreeDllHandle(UE4x360ceHandle);
		UE4x360ceHandle = nullptr;
	}

	delete ue4x360ce;
	ue4x360ce = nullptr;
}

void FDroneController::Tick(float DeltaTime)
{
	this->DeltaTime = DeltaTime;
}

void FDroneController::SendControllerEvents()
{
	UE_LOG(DroneControllerModule, Warning, TEXT("ue4x360ce->GetDeviceNum %d"), ue4x360ce->ue4x360ceFrame.ControllersCount);
	// Just for Debugging
	{
		MessageHandler->OnControllerAnalog(DroneControllerKeyNames::DroneController_Stick_Left_X, 0, 0.f);
	}
}

void FDroneController::SetMessageHandler(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
{
	MessageHandler = InMessageHandler;
}

void FDroneController::SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value)
{
	//UE_LOG(DroneControllerModule, Warning, TEXT("FDroneController::SetChannelValue"));
}

void FDroneController::SetChannelValues(int32 ControllerId, const FForceFeedbackValues & Values)
{
	//UE_LOG(DroneControllerModule, Warning, TEXT("FDroneController::SetChannelValues"));
}

bool FDroneController::Exec(UWorld * InWorld, const TCHAR * Cmd, FOutputDevice & Ar)
{
	return false;
}

bool FDroneController::IsGamepadAttached() const
{
	return true;
}


#undef LOCTEXT_NAMESPACE
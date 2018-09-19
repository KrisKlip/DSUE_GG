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
{
	UE_LOG(DroneControllerModule, Warning, TEXT("FDroneController::FDroneController"));

#if PLATFORM_64BITS
	FString WinDir = TEXT("Win64/");
#else
	FString WinDir = TEXT("Win32/");
#endif

	void* xinputDLLHandle = nullptr;
	void* vldDLLHandle = nullptr;
	FString RootXInputPath = FPaths::ProjectDir() / FString::Printf(TEXT("Source/DroneControllerModule/Binaries/")) / WinDir;

	FPlatformProcess::PushDllDirectory(*RootXInputPath);
	vldDLLHandle = FPlatformProcess::GetDllHandle(*(RootXInputPath + "vld/vld_x64.dll"));
	xinputDLLHandle = FPlatformProcess::GetDllHandle(*(RootXInputPath + "xinput/xinput1_3.dll"));
	FPlatformProcess::PopDllDirectory(*RootXInputPath);

	if (!vldDLLHandle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load vld library."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Success to load vld library."));
	}

	if (!xinputDLLHandle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load xinput1_3 library."));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Success to load xinput1_3 library."));
	}


	EKeys::AddMenuCategoryDisplayInfo("DroneController", LOCTEXT("DroneControllerSubCateogry", "DroneController"), TEXT("GraphEditor.PadEvent_16x"));

	// Add buttons
	Buttons[0][EDroneControllerButton::Hook] = DroneControllerKeyNames::Hook;

	// Add button keys
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::Hook, LOCTEXT("DroneController_Hook", "DroneController Hook"), FKeyDetails::GamepadKey, "DroneController"));

	// Float axis
	EKeys::AddKey(FKeyDetails(DroneControllerKeys::DroneController_Stick_Left_X, LOCTEXT("DroneController_Stick_Left_X", "DroneController Left Stick X"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "DroneController"));
}

FDroneController::~FDroneController()
{
	//UE_LOG(DroneControllerModule, Warning, TEXT("FDroneController::~FDroneController"));
}

void FDroneController::Tick(float DeltaTime)
{
	this->DeltaTime = DeltaTime;
}

void FDroneController::SendControllerEvents()
{
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

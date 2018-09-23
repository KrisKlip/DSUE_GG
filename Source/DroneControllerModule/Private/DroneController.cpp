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
#include "DroneControllerDI.h"

#define LOCTEXT_NAMESPACE "DroneController"


namespace DroneControllerKeyNames
{
	const FGamepadKeyNames::Type FaceButtonBottom("DroneController_FaceButtonBottom");
	const FGamepadKeyNames::Type FaceButtonRight("DroneController_FaceButtonRight");
	const FGamepadKeyNames::Type FaceButtonLeft("DroneController_FaceButtonLeft");
	const FGamepadKeyNames::Type FaceButtonTop("DroneController_FaceButtonTop");
	const FGamepadKeyNames::Type LeftShoulder("DroneController_LeftShoulder");
	const FGamepadKeyNames::Type RightShoulder("DroneController_RightShoulder");
	const FGamepadKeyNames::Type SpecialRight("DroneController_SpecialRight");
	const FGamepadKeyNames::Type SpecialLeft("DroneController_SpecialLeft");
	const FGamepadKeyNames::Type LeftThumb("DroneController_LeftThumb");
	const FGamepadKeyNames::Type RightThumb("DroneController_RightThumb");
	const FGamepadKeyNames::Type LeftTriggerThreshold("DroneController_LeftTriggerThreshold");
	const FGamepadKeyNames::Type RightTriggerThreshold("DroneController_RightTriggerThreshold");
	const FGamepadKeyNames::Type DPadUp("DroneController_DPadUp");
	const FGamepadKeyNames::Type DPadDown("DroneController_DPadDown");
	const FGamepadKeyNames::Type DPadLeft("DroneController_DPadLeft");
	const FGamepadKeyNames::Type DPadRight("DroneController_DPadRight");
	const FGamepadKeyNames::Type LeftStickUp("DroneController_LeftStickUp");
	const FGamepadKeyNames::Type LeftStickDown("DroneController_LeftStickDown");
	const FGamepadKeyNames::Type LeftStickLeft("DroneController_LeftStickLeft");
	const FGamepadKeyNames::Type LeftStickRight("DroneController_LeftStickRight");
	const FGamepadKeyNames::Type RightStickUp("DroneController_RightStickUp");
	const FGamepadKeyNames::Type RightStickDown("DroneController_RightStickDown");
	const FGamepadKeyNames::Type RightStickLeft("DroneController_RightStickLeft");
	const FGamepadKeyNames::Type RightStickRight("DroneController_RightStickRight");

	const FGamepadKeyNames::Type LeftAnalogX("DroneController_LeftAnalogX");
	const FGamepadKeyNames::Type LeftAnalogY("DroneController_LeftAnalogY");
	const FGamepadKeyNames::Type RightAnalogX("DroneController_RightAnalogX");
	const FGamepadKeyNames::Type RightAnalogY("DroneController_RightAnalogY");
	const FGamepadKeyNames::Type LeftTriggerAnalog("DroneController_LeftTriggerAnalog");
	const FGamepadKeyNames::Type RightTriggerAnalog("DroneController_RightTriggerAnalog");
}

namespace DroneControllerKeys
{
	const FKey LeftAnalogX("DroneController_LeftAnalogX");
	const FKey LeftAnalogY("DroneController_LeftAnalogY");
	const FKey RightAnalogX("DroneController_RightAnalogX");
	const FKey RightAnalogY("DroneController_RightAnalogY");
	const FKey LeftTriggerAnalog("DroneController_LeftTriggerAnalog");
	const FKey RightTriggerAnalog("DroneController_RightTriggerAnalog");
}

static float ShortToNormalizedFloat(int16 AxisVal)
{
	// normalize [-32768..32767] -> [-1..1]
	const float Norm = (AxisVal <= 0 ? 32768.f : 32767.f);
	return float(AxisVal) / Norm;
}



FDroneController::FDroneController(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler)
	: MessageHandler(InMessageHandler)
	, bIsSendControllerEvents(false)
	, DeltaTime(0.f)
	, DroneControllerDI(nullptr)
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

	// Set all controllers, 4 by default
	for (int32 ControllerIndex = 0; ControllerIndex < DRONE_MAX_NUM_XINPUT_CONTROLLERS; ++ControllerIndex)
	{
		FControllerState& ControllerState = ControllerStates[ControllerIndex];
		FMemory::Memzero(&ControllerState, sizeof(FControllerState));

		// TODO. Batname. Fer new send all signals to  same 0 indx controller
		ControllerState.ControllerId = 0;// ControllerIndex;
	}

	bIsGamepadAttached = false;
	bNeedsControllerStateUpdate = true;
	InitialButtonRepeatDelay = 0.2f;
	ButtonRepeatDelay = 0.1f;

	// In the engine, all controllers map to xbox controllers for consistency 
	X360ToXboxControllerMapping[0] = 0;		// A
	X360ToXboxControllerMapping[1] = 1;		// B
	X360ToXboxControllerMapping[2] = 2;		// X
	X360ToXboxControllerMapping[3] = 3;		// Y
	X360ToXboxControllerMapping[4] = 4;		// L1
	X360ToXboxControllerMapping[5] = 5;		// R1
	X360ToXboxControllerMapping[6] = 7;		// Back 
	X360ToXboxControllerMapping[7] = 6;		// Start
	X360ToXboxControllerMapping[8] = 8;		// Left thumbstick
	X360ToXboxControllerMapping[9] = 9;		// Right thumbstick
	X360ToXboxControllerMapping[10] = 10;	// L2
	X360ToXboxControllerMapping[11] = 11;	// R2
	X360ToXboxControllerMapping[12] = 12;	// Dpad up
	X360ToXboxControllerMapping[13] = 13;	// Dpad down
	X360ToXboxControllerMapping[14] = 14;	// Dpad left
	X360ToXboxControllerMapping[15] = 15;	// Dpad right
	X360ToXboxControllerMapping[16] = 16;	// Left stick up
	X360ToXboxControllerMapping[17] = 17;	// Left stick down
	X360ToXboxControllerMapping[18] = 18;	// Left stick left
	X360ToXboxControllerMapping[19] = 19;	// Left stick right
	X360ToXboxControllerMapping[20] = 20;	// Right stick up
	X360ToXboxControllerMapping[21] = 21;	// Right stick down
	X360ToXboxControllerMapping[22] = 22;	// Right stick left
	X360ToXboxControllerMapping[23] = 23;	// Right stick right

	Buttons[0] = DroneControllerKeyNames::FaceButtonBottom;
	Buttons[1] = DroneControllerKeyNames::FaceButtonRight;
	Buttons[2] = DroneControllerKeyNames::FaceButtonLeft;
	Buttons[3] = DroneControllerKeyNames::FaceButtonTop;
	Buttons[4] = DroneControllerKeyNames::LeftShoulder;
	Buttons[5] = DroneControllerKeyNames::RightShoulder;
	Buttons[6] = DroneControllerKeyNames::SpecialRight;
	Buttons[7] = DroneControllerKeyNames::SpecialLeft;
	Buttons[8] = DroneControllerKeyNames::LeftThumb;
	Buttons[9] = DroneControllerKeyNames::RightThumb;
	Buttons[10] = DroneControllerKeyNames::LeftTriggerThreshold;
	Buttons[11] = DroneControllerKeyNames::RightTriggerThreshold;
	Buttons[12] = DroneControllerKeyNames::DPadUp;
	Buttons[13] = DroneControllerKeyNames::DPadDown;
	Buttons[14] = DroneControllerKeyNames::DPadLeft;
	Buttons[15] = DroneControllerKeyNames::DPadRight;
	Buttons[16] = DroneControllerKeyNames::LeftStickUp;
	Buttons[17] = DroneControllerKeyNames::LeftStickDown;
	Buttons[18] = DroneControllerKeyNames::LeftStickLeft;
	Buttons[19] = DroneControllerKeyNames::LeftStickRight;
	Buttons[20] = DroneControllerKeyNames::RightStickUp;
	Buttons[21] = DroneControllerKeyNames::RightStickDown;
	Buttons[22] = DroneControllerKeyNames::RightStickLeft;
	Buttons[23] = DroneControllerKeyNames::RightStickRight;

	// Add category in input config
	EKeys::AddMenuCategoryDisplayInfo("DroneController", LOCTEXT("DroneControllerSubCateogry", "DroneController"), TEXT("GraphEditor.PadEvent_16x"));

	// Add button keys
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::FaceButtonBottom, LOCTEXT("DroneController_FaceButtonBottom", "DroneController FaceButtonBottom"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::FaceButtonRight, LOCTEXT("DroneController_FaceButtonRight", "DroneController FaceButtonRight"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::FaceButtonLeft, LOCTEXT("DroneController_FaceButtonLeft", "DroneController FaceButtonLeft"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::FaceButtonTop, LOCTEXT("DroneController_FaceButtonTop", "DroneController FaceButtonTop"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::LeftShoulder, LOCTEXT("DroneController_LeftShoulder", "DroneController LeftShoulder"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::RightShoulder, LOCTEXT("DroneController_RightShoulder", "DroneController RightShoulder"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::SpecialRight, LOCTEXT("DroneController_SpecialRight", "DroneController SpecialRight"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::SpecialLeft, LOCTEXT("DroneController_SpecialLeft", "DroneController SpecialLeft"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::LeftThumb, LOCTEXT("DroneController_LeftThumb", "DroneController LeftThumb"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::RightThumb, LOCTEXT("DroneController_RightThumb", "DroneController RightThumb"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::LeftTriggerThreshold, LOCTEXT("DroneController_LeftTriggerThreshold", "DroneController LeftTriggerThreshold"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::RightTriggerThreshold, LOCTEXT("DroneController_RightTriggerThreshold", "DroneController RightTriggerThreshold"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::DPadUp, LOCTEXT("DroneController_DPadUp", "DroneController DPadUp"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::DPadDown, LOCTEXT("DroneController_DPadDown", "DroneController DPadDown"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::DPadLeft, LOCTEXT("DroneController_DPadLeft", "DroneController DPadLeft"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::DPadRight, LOCTEXT("DroneController_DPadRight", "DroneController DPadRight"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::LeftStickUp, LOCTEXT("DroneController_LeftStickUp", "DroneController LeftStickUp"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::LeftStickDown, LOCTEXT("DroneController_LeftStickDown", "DroneController LeftStickDown"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::LeftStickLeft, LOCTEXT("DroneController_LeftStickLeft", "DroneController LeftStickLeft"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::LeftStickRight, LOCTEXT("DroneController_LeftStickRight", "DroneController LeftStickRight"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::RightStickUp, LOCTEXT("DroneController_RightStickUp", "DroneController RightStickUp"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::RightStickDown, LOCTEXT("DroneController_RightStickDown", "DroneController RightStickDown"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::RightStickLeft, LOCTEXT("DroneController_RightStickLeft", "DroneController RightStickLeft"), FKeyDetails::GamepadKey, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeyNames::RightStickRight, LOCTEXT("DroneController_RightStickRight", "DroneController RightStickRight"), FKeyDetails::GamepadKey, "DroneController"));


	// Float axis
	EKeys::AddKey(FKeyDetails(DroneControllerKeys::LeftAnalogX, LOCTEXT("DroneController_LeftAnalogX", "DroneController LeftAnalogX"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeys::LeftAnalogY, LOCTEXT("DroneController_LeftAnalogY", "DroneController LeftAnalogY"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeys::RightAnalogX, LOCTEXT("DroneController_RightAnalogX", "DroneController RightAnalogX"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeys::RightAnalogY, LOCTEXT("DroneController_RightAnalogY", "DroneController RightAnalogY"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeys::LeftTriggerAnalog, LOCTEXT("DroneController_LeftTriggerAnalog", "DroneController LeftTriggerAnalog"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "DroneController"));
	EKeys::AddKey(FKeyDetails(DroneControllerKeys::RightTriggerAnalog, LOCTEXT("DroneController_RightTriggerAnalog", "DroneController RightTriggerAnalog"), FKeyDetails::GamepadKey | FKeyDetails::FloatAxis, "DroneController"));
	
	// Read config controller data and init controller
	{
		FString guidProduct;
		FString ProductName;

		GConfig->GetString(TEXT("SystemSettings"),
			TEXT("drone.controller.default.guidProduct"),
			guidProduct,
			GEngineIni);

		GConfig->GetString(TEXT("SystemSettings"),
			TEXT("drone.controller.defalut.ProductName"),
			ProductName,
			GEngineIni);

		// Init controller lib
		DroneControllerDI = new FDroneControllerDI();
		DroneControllerDI->Run({ -1, std::string(TCHAR_TO_UTF8(*guidProduct)), std::string(TCHAR_TO_UTF8(*ProductName)), true});
	}
}

FDroneController::~FDroneController()
{
	if (UE4x360ceHandle)
	{
		FPlatformProcess::FreeDllHandle(UE4x360ceHandle);
		UE4x360ceHandle = nullptr;
	}

	delete DroneControllerDI;
	DroneControllerDI = nullptr;
}

void FDroneController::Tick(float DeltaTime)
{
	this->DeltaTime = DeltaTime;
}

void FDroneController::SendControllerEvents()
{
	DroneControllerDI->UpdateDevices();

	// TODO. Batname. fix it. now it update controllers state each frame.
	bNeedsControllerStateUpdate = true;

	bool bWereConnected[DRONE_MAX_NUM_XINPUT_CONTROLLERS];
	XINPUT_STATE XInputStates[DRONE_MAX_NUM_XINPUT_CONTROLLERS];

	bIsGamepadAttached = false;
	for (int32 ControllerIndex = 0; ControllerIndex < DRONE_MAX_NUM_XINPUT_CONTROLLERS; ++ControllerIndex)
	{
		FControllerState& ControllerState = ControllerStates[ControllerIndex];

		bWereConnected[ControllerIndex] = ControllerState.bIsConnected;

		if (ControllerState.bIsConnected || bNeedsControllerStateUpdate)
		{
			XINPUT_STATE& XInputState = XInputStates[ControllerIndex];
			FMemory::Memzero(&XInputState, sizeof(XINPUT_STATE));

			ControllerState.bIsConnected = (UE4x360ce::XInputGetState(ControllerIndex, &XInputState) == ERROR_SUCCESS) ? true : false;

			if (ControllerState.bIsConnected)
			{
				bIsGamepadAttached = true;
			}
		}
	}


	for (int32 ControllerIndex = 0; ControllerIndex < DRONE_MAX_NUM_XINPUT_CONTROLLERS; ++ControllerIndex)
	{
		FControllerState& ControllerState = ControllerStates[ControllerIndex];

		const bool bWasConnected = bWereConnected[ControllerIndex];

		// If the controller is connected send events or if the controller was connected send a final event with default states so that 
		// the game doesn't think that controller buttons are still held down
		if (ControllerState.bIsConnected || bWasConnected)
		{
			const XINPUT_STATE& XInputState = XInputStates[ControllerIndex];

			// If the controller is connected now but was not before, refresh the information
			if (!bWasConnected && ControllerState.bIsConnected)
			{
				FCoreDelegates::OnControllerConnectionChange.Broadcast(true, -1, ControllerState.ControllerId);
			}
			else if (bWasConnected && !ControllerState.bIsConnected)
			{
				FCoreDelegates::OnControllerConnectionChange.Broadcast(false, -1, ControllerState.ControllerId);
			}

			bool CurrentStates[DRONE_MAX_NUM_CONTROLLER_BUTTONS] = { 0 };

			// Get the current state of all buttons
			CurrentStates[X360ToXboxControllerMapping[0]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_A);
			CurrentStates[X360ToXboxControllerMapping[1]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_B);
			CurrentStates[X360ToXboxControllerMapping[2]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_X);
			CurrentStates[X360ToXboxControllerMapping[3]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
			CurrentStates[X360ToXboxControllerMapping[4]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
			CurrentStates[X360ToXboxControllerMapping[5]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
			CurrentStates[X360ToXboxControllerMapping[6]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
			CurrentStates[X360ToXboxControllerMapping[7]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_START);
			CurrentStates[X360ToXboxControllerMapping[8]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
			CurrentStates[X360ToXboxControllerMapping[9]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
			CurrentStates[X360ToXboxControllerMapping[10]] = !!(XInputState.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			CurrentStates[X360ToXboxControllerMapping[11]] = !!(XInputState.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			CurrentStates[X360ToXboxControllerMapping[12]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
			CurrentStates[X360ToXboxControllerMapping[13]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
			CurrentStates[X360ToXboxControllerMapping[14]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
			CurrentStates[X360ToXboxControllerMapping[15]] = !!(XInputState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
			CurrentStates[X360ToXboxControllerMapping[16]] = !!(XInputState.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			CurrentStates[X360ToXboxControllerMapping[17]] = !!(XInputState.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			CurrentStates[X360ToXboxControllerMapping[18]] = !!(XInputState.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			CurrentStates[X360ToXboxControllerMapping[19]] = !!(XInputState.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			CurrentStates[X360ToXboxControllerMapping[20]] = !!(XInputState.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			CurrentStates[X360ToXboxControllerMapping[21]] = !!(XInputState.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			CurrentStates[X360ToXboxControllerMapping[22]] = !!(XInputState.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			CurrentStates[X360ToXboxControllerMapping[23]] = !!(XInputState.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

			// Send new analog data if it's different or outside the platform deadzone.
			auto OnControllerAnalog = [this, &ControllerState](const FName& GamePadKey, const auto NewAxisValue, const float NewAxisValueNormalized, auto& OldAxisValue, const auto DeadZone) {
				if (OldAxisValue != NewAxisValue || FMath::Abs((int32)NewAxisValue) > DeadZone)
				{
					MessageHandler->OnControllerAnalog(GamePadKey, ControllerState.ControllerId, NewAxisValueNormalized);
				}
				OldAxisValue = NewAxisValue;
			};

			const auto& Gamepad = XInputState.Gamepad;

			OnControllerAnalog(DroneControllerKeyNames::LeftAnalogX, Gamepad.sThumbLX, ShortToNormalizedFloat(Gamepad.sThumbLX), ControllerState.LeftXAnalog, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
			OnControllerAnalog(DroneControllerKeyNames::LeftAnalogY, Gamepad.sThumbLY, ShortToNormalizedFloat(Gamepad.sThumbLY), ControllerState.LeftYAnalog, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

			OnControllerAnalog(DroneControllerKeyNames::RightAnalogX, Gamepad.sThumbRX, ShortToNormalizedFloat(Gamepad.sThumbRX), ControllerState.RightXAnalog, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
			OnControllerAnalog(DroneControllerKeyNames::RightAnalogY, Gamepad.sThumbRY, ShortToNormalizedFloat(Gamepad.sThumbRY), ControllerState.RightYAnalog, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

			OnControllerAnalog(DroneControllerKeyNames::LeftTriggerAnalog, Gamepad.bLeftTrigger, Gamepad.bLeftTrigger / 255.f, ControllerState.LeftTriggerAnalog, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
			OnControllerAnalog(DroneControllerKeyNames::RightTriggerAnalog, Gamepad.bRightTrigger, Gamepad.bRightTrigger / 255.f, ControllerState.RightTriggerAnalog, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);

			const double CurrentTime = FPlatformTime::Seconds();

			// For each button check against the previous state and send the correct message if any
			for (int32 ButtonIndex = 0; ButtonIndex < DRONE_MAX_NUM_CONTROLLER_BUTTONS; ++ButtonIndex)
			{
				if (CurrentStates[ButtonIndex] != ControllerState.ButtonStates[ButtonIndex])
				{
					if (CurrentStates[ButtonIndex])
					{
						MessageHandler->OnControllerButtonPressed(Buttons[ButtonIndex], ControllerState.ControllerId, false);
					}
					else
					{
						MessageHandler->OnControllerButtonReleased(Buttons[ButtonIndex], ControllerState.ControllerId, false);
					}

					if (CurrentStates[ButtonIndex] != 0)
					{
						// this button was pressed - set the button's NextRepeatTime to the InitialButtonRepeatDelay
						ControllerState.NextRepeatTime[ButtonIndex] = CurrentTime + InitialButtonRepeatDelay;
					}
				}
				else if (CurrentStates[ButtonIndex] != 0 && ControllerState.NextRepeatTime[ButtonIndex] <= CurrentTime)
				{
					MessageHandler->OnControllerButtonPressed(Buttons[ButtonIndex], ControllerState.ControllerId, true);

					// set the button's NextRepeatTime to the ButtonRepeatDelay
					ControllerState.NextRepeatTime[ButtonIndex] = CurrentTime + ButtonRepeatDelay;
				}

				// Update the state for next time
				ControllerState.ButtonStates[ButtonIndex] = CurrentStates[ButtonIndex];
			}

			// apply force feedback
			XINPUT_VIBRATION VibrationState;

			const float LargeValue = (ControllerState.ForceFeedback.LeftLarge > ControllerState.ForceFeedback.RightLarge ? ControllerState.ForceFeedback.LeftLarge : ControllerState.ForceFeedback.RightLarge);
			const float SmallValue = (ControllerState.ForceFeedback.LeftSmall > ControllerState.ForceFeedback.RightSmall ? ControllerState.ForceFeedback.LeftSmall : ControllerState.ForceFeedback.RightSmall);

			VibrationState.wLeftMotorSpeed = (::WORD) (LargeValue * 65535.0f);
			VibrationState.wRightMotorSpeed = (::WORD) (SmallValue * 65535.0f);

			// TODO. Batname. We need use emulator for it
			// XInputSetState((::DWORD) ControllerState.ControllerId, &VibrationState);
		}
	}

	bNeedsControllerStateUpdate = false;
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

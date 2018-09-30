#pragma once

#include "CoreMinimal.h"
#include "IInputDevice.h"
#include "IInputDeviceModule.h"
#include "IInputInterface.h"
#include "DroneControllerStates.h"

class FDroneControllerDI;

class FDroneController : public IInputDevice
{
public:
	FDroneController(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler);
	virtual ~FDroneController();
	virtual void Tick( float DeltaTime ) override;
	virtual void SendControllerEvents() override;
	virtual void SetMessageHandler(const TSharedRef< FGenericApplicationMessageHandler >& InMessageHandler) override;
	virtual void SetChannelValue(int32 ControllerId, FForceFeedbackChannelType ChannelType, float Value) override;
	virtual void SetChannelValues(int32 ControllerId, const FForceFeedbackValues &Values) override;
	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
	virtual bool IsGamepadAttached() const override;

	static void PreInit();

public:
	const TArray<FDroneControllerState>& GetDroneControllerStates() const { return ControllerStates; }

// Bluprint functional libary accessable
public:
	bool bIsSendControllerEvents;

private:
	/** handler to send all messages to */
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;

	float DeltaTime;

private:
	FDroneControllerDI* DroneControllerDI;
	void* UE4x360ceHandle = nullptr;

	bool bIsGamepadAttached;

	/** In the engine, all controllers map to xbox controllers for consistency */
	uint8	X360ToXboxControllerMapping[DRONE_MAX_NUM_CONTROLLER_BUTTONS];

	/** Controller states */
	TArray<FDroneControllerState> ControllerStates;

	/** Delay before sending a repeat message after a button was first pressed */
	float InitialButtonRepeatDelay;

	/** Delay before sending a repeat message after a button has been pressed for a while */
	float ButtonRepeatDelay;

	/**  */
	FGamepadKeyNames::Type Buttons[DRONE_MAX_NUM_CONTROLLER_BUTTONS];
};
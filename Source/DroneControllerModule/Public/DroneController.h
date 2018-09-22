#pragma once

#include "CoreMinimal.h"
#include "IInputDevice.h"
#include "IInputDeviceModule.h"
#include "IInputInterface.h"

class FDroneControllerDI;

/** Max number of controllers. */
#define DRONE_MAX_NUM_XINPUT_CONTROLLERS 4

/** Max number of controller buttons.  Must be < 256*/
#define DRONE_MAX_NUM_CONTROLLER_BUTTONS 24

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

	struct FControllerState
	{
		/** Last frame's button states, so we only send events on edges */
		bool ButtonStates[DRONE_MAX_NUM_CONTROLLER_BUTTONS];

		/** Next time a repeat event should be generated for each button */
		double NextRepeatTime[DRONE_MAX_NUM_CONTROLLER_BUTTONS];

		/** Raw Left thumb x analog value */
		int16 LeftXAnalog;

		/** Raw left thumb y analog value */
		int16 LeftYAnalog;

		/** Raw Right thumb x analog value */
		int16 RightXAnalog;

		/** Raw Right thumb x analog value */
		int16 RightYAnalog;

		/** Left Trigger analog value */
		uint8 LeftTriggerAnalog;

		/** Right trigger analog value */
		uint8 RightTriggerAnalog;

		/** Id of the controller */
		int32 ControllerId;

		/** If the controller is currently connected */
		bool bIsConnected;

		/** Current force feedback values */
		FForceFeedbackValues ForceFeedback;
	};

	/** If we've been notified by the system that the controller state may have changed */
	bool bNeedsControllerStateUpdate;

	bool bIsGamepadAttached;

	/** In the engine, all controllers map to xbox controllers for consistency */
	uint8	X360ToXboxControllerMapping[DRONE_MAX_NUM_CONTROLLER_BUTTONS];

	/** Controller states */
	FControllerState ControllerStates[DRONE_MAX_NUM_XINPUT_CONTROLLERS];

	/** Delay before sending a repeat message after a button was first pressed */
	float InitialButtonRepeatDelay;

	/** Delay before sending a repeat message after a button has been pressed for a while */
	float ButtonRepeatDelay;

	/**  */
	FGamepadKeyNames::Type Buttons[DRONE_MAX_NUM_CONTROLLER_BUTTONS];
};
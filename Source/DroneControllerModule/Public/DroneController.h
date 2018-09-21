#pragma once

#include "CoreMinimal.h"
#include "IInputDevice.h"
#include "IInputDeviceModule.h"

class FDroneControllerDI;

class FDroneController : public IInputDevice
{
public:
	/**
	* Buttons on the SteamVR controller
	*/
	struct EDroneControllerButton
	{
		enum Type
		{
			Hook,

			/** Max number of controller buttons.  Must be < 256 */
			TotalButtonCount
		};
	};

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
	/** Mapping of controller buttons */
	FGamepadKeyNames::Type Buttons[1][EDroneControllerButton::TotalButtonCount];

	/** handler to send all messages to */
	TSharedRef<FGenericApplicationMessageHandler> MessageHandler;

	float DeltaTime;

private:
	FDroneControllerDI* DroneControllerDI;
	void* UE4x360ceHandle = nullptr;
};
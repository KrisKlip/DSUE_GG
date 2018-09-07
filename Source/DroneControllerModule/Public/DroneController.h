#pragma once

#include "CoreMinimal.h"
#include "HAL/PlatformTime.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericApplicationMessageHandler.h"
#include "Modules/ModuleManager.h"
#include "GenericPlatform/IInputInterface.h"
#include "IInputDevice.h"
#include "IInputDeviceModule.h"
#include "IDroneControllerModule.h"
#include "DroneControllerModule.h"

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
};
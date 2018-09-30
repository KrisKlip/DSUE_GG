#pragma once

#include "CoreMinimal.h"
#include "DroneControllerStates.generated.h"

/** Max number of controllers. */
#define DRONE_MAX_NUM_XINPUT_CONTROLLERS 4

/** Max number of controller buttons.  Must be < 256*/
#define DRONE_MAX_NUM_CONTROLLER_BUTTONS 24

USTRUCT(BlueprintType)
struct FDroneControllerState
{
	GENERATED_BODY()

	/** Last frame's button states, so we only send events on edges */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	TArray<bool> ButtonStates;

	/** Next time a repeat event should be generated for each button */
	double NextRepeatTime[DRONE_MAX_NUM_CONTROLLER_BUTTONS];

	/** Raw Left thumb x analog value */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	int32 LeftXAnalog = 0;

	/** Raw left thumb y analog value */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	int32 LeftYAnalog = 0;

	/** Raw Right thumb x analog value */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	int32 RightXAnalog = 0;

	/** Raw Right thumb x analog value */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	int32 RightYAnalog = 0;

	/** Left Trigger analog value */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	uint8 LeftTriggerAnalog = 0;

	/** Right trigger analog value */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	uint8 RightTriggerAnalog = 0;

	/** Id of the controller */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	int32 ControllerId = 0;

	/** If the controller is currently connected */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	bool bIsConnected = false;

	/** Get raw xinput button state */
	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	int32 RawButtonsState = 0;

	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	FString guidInstance;

	UPROPERTY(BlueprintReadWrite, Category = "DroneControllerState")
	FString ProductName;

	/** Current force feedback values */
	FForceFeedbackValues ForceFeedback;
};
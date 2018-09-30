// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DroneControllerStates.h"
#include "DroneControllerBFL.generated.h"

/**
 * 
 */
UCLASS()
class DRONECONTROLLERMODULE_API UDroneControllerBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "DroneController")
	static bool SetSendControllerEvents(bool bIsSendControllerEvents);

	UFUNCTION(BlueprintCallable, Category = "DroneController")
	static TArray<FDroneControllerState> GetDroneControllerStates();
};

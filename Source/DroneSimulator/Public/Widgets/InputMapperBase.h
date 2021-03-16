// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputMapperBase.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_DELEGATE_OneParam(FInputAxisDelegate, float, value);

UCLASS()
class DRONESIMULATOR_API UInputMapperBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Input")
		void ListenForAxisEvent(FName axis, FInputAxisDelegate func);
	UFUNCTION(BlueprintCallable, Category = "Input")
		void StopListenForAxisEvent(FName axis);

	void OnAxisValue(float value, FInputAxisDelegate func);

};

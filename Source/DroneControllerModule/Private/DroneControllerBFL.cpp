// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneControllerBFL.h"
#include "DroneController.h"
#include "IDroneControllerModule.h"

#include "IConsoleManager.h"

bool UDroneControllerBFL::SetSendControllerEvents(bool bIsSendControllerEvents)
{
	auto DroneControllerRef = IDroneControllerModule::Get().GetDroneControllerRef();
	DroneControllerRef->bIsSendControllerEvents = bIsSendControllerEvents;

	return true;
}

TArray<FDroneControllerState> UDroneControllerBFL::GetDroneControllerStates()
{
	auto DroneControllerRef = IDroneControllerModule::Get().GetDroneControllerRef();

	return DroneControllerRef->GetDroneControllerStates();
}

int32 UDroneControllerBFL::GetDroneControllerMode()
{
	static const auto CVarDroneControllerMode = IConsoleManager::Get().FindConsoleVariable(TEXT("drone.controller.mode"));
	return CVarDroneControllerMode->GetInt();
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "DroneControllerBFL.h"
#include "DroneController.h"
#include "IDroneControllerModule.h"

bool UDroneControllerBFL::SetSendControllerEvents(bool bIsSendControllerEvents)
{
	auto DroneControllerRef = IDroneControllerModule::Get().GetDroneControllerRef();
	DroneControllerRef->bIsSendControllerEvents = bIsSendControllerEvents;

	return true;
}




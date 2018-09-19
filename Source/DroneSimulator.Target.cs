// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class DroneSimulatorTarget : TargetRules
{
	public DroneSimulatorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
        bUsesSteam = true;
        ExtraModuleNames.AddRange( new string[] { "DroneSimulator" } );
        ExtraModuleNames.Add("DroneControllerModule");
    }
}
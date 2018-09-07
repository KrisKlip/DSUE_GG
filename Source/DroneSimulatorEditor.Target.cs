// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class DroneSimulatorEditorTarget : TargetRules
{
	public DroneSimulatorEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		ExtraModuleNames.Add("DroneSimulator");
    }
}

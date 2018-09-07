using UnrealBuildTool;
 
public class DroneControllerModule : ModuleRules
{
	public DroneControllerModule(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "InputDevice" });
 
		//The path for the header files
		PublicIncludePaths.AddRange(new string[] {"DroneControllerModule/Public"});
 
		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {"DroneControllerModule/Private"});
	}
}
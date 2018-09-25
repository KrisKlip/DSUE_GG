using System.IO;
using UnrealBuildTool;

public class DroneControllerModule : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "ThirdParty/")); }
    }

    private string BinariesPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "Binaries/")); }
    }

    private string IncludePath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "Include/")); }
    }

    public DroneControllerModule(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "InputDevice" });
 
		//The path for the header files
		PublicIncludePaths.AddRange(new string[] {
            "DroneControllerModule/Public",
        });
 
		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {
            "DroneControllerModule/Private",
             Path.Combine(IncludePath, "UE4x360ce")
        });

        LoadLib(Target);
    }

    public bool LoadLib(ReadOnlyTargetRules Target)
    {
        bool isLibrarySupported = false;
        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibrarySupported = true;

            string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "Win64" : "Win32";

            //  lib
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, PlatformString, "UE4x360ce", "UE4x360ce.lib"));
            PublicAdditionalLibraries.Add("atls.lib");

            // Add dll
            PublicDelayLoadDLLs.Add("UE4x360ce.dll");
            RuntimeDependencies.Add(Path.Combine(BinariesPath, PlatformString, "UE4x360ce", "UE4x360ce.dll"));
        }

        return isLibrarySupported;
    }
}
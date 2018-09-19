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

            // vld
            Path.Combine(IncludePath, "vld", "vld.h"),
            Path.Combine(IncludePath, "vld", "vld_def.h"),
        });
 
		//The path for the source files
		PrivateIncludePaths.AddRange(new string[] {"DroneControllerModule/Private"});

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
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, PlatformString, "xinput", "xinput1_3.lib"));
            PublicAdditionalLibraries.Add(Path.Combine(ThirdPartyPath, PlatformString, "vld", "vld.lib"));


            // Add dll
            PublicDelayLoadDLLs.Add("xinput1_3.dll");
            PublicDelayLoadDLLs.Add("vld_x64.dll");

            RuntimeDependencies.Add(Path.Combine(BinariesPath, PlatformString, "xinput", "xinput1_3.dll"));
            RuntimeDependencies.Add(Path.Combine(BinariesPath, PlatformString, "vld", "vld_x64.dll"));
        }

        return isLibrarySupported;
    }
}
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class vtm : ModuleRules
{
	public vtm(ReadOnlyTargetRules Target) : base(Target)
	{
        bUseRTTI = true; // dynamic_cast

        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });

        PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        PublicDependencyModuleNames.AddRange(new string[] { "Json" });

        PublicDependencyModuleNames.AddRange(new string[] { "UMG" });

        PublicDependencyModuleNames.AddRange(new string[] { "NavigationSystem" });

        LoadLua(Target);
    }

    private string LibsPath
    {
        get { return System.IO.Path.GetFullPath(System.IO.Path.Combine(ModuleDirectory, "../../Libs/")); }
    }

    private bool LoadLua(ReadOnlyTargetRules Target)
    {
        bool isLibSupported = false;

        // Check if we are on Windows
        if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
        {
            isLibSupported = true;

            //string PlatformString = (Target.Platform == UnrealTargetPlatform.Win64) ? "x64" : "x86";
            //string LibrariesPath = System.IO.Path.Combine(LibsPath, "Lua", "libraries");

            string LibrariesPath = System.IO.Path.Combine(LibsPath, "Lua");
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(LibrariesPath, "lua53.lib"));
            PublicIncludePaths.Add(System.IO.Path.Combine(LibsPath, "Lua", "include"));

            LibrariesPath = System.IO.Path.Combine(LibsPath, "luacppinterface");
            PublicAdditionalLibraries.Add(System.IO.Path.Combine(LibrariesPath, "LuaCppInterface.lib"));
            PublicIncludePaths.Add(System.IO.Path.Combine(LibsPath, "luacppinterface", "include"));

        }

        PublicDefinitions.Add(string.Format("WITH_LUA_BINDING={0}", isLibSupported ? 1 : 0));

        return isLibSupported; 
    }
}

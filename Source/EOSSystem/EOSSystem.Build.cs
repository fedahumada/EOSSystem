// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EOSSystem : ModuleRules
{
	public EOSSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { 
	        "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay",			//~ Defaults
            "NavigationSystem", "AIModule", "Niagara", "EnhancedInput",					//~ Misc
            "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemEOS",			//~ EOS
            "PlayFab", "PlayFabGSDK"													//~ PlayFab
        });
    }
}

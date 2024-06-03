// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EOSSystem : ModuleRules
{
	public EOSSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
	        new string[] {
	        "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", 
	        "AIModule", "Niagara", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemEOS" , 
	        "VoiceChat"
	        
        });
    }
}

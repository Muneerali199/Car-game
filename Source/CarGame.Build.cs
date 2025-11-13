// CarGame.Build.cs
// Build configuration for CarGame module
// Copyright 2025. All Rights Reserved.

using UnrealBuildTool;

public class CarGame : ModuleRules
{
	public CarGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore",
			"PhysicsCore",
			"ChaosVehicles",
			"Niagara",
			"AudioMixer"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"Slate",
			"SlateCore",
			"UMG",
			"OnlineSubsystem",
			"OnlineSubsystemUtils",
			"AIModule"
		});

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your .uproject file with the Enabled attribute set to true

		// Optimization settings
		if (Target.Configuration == UnrealTargetConfiguration.Shipping)
		{
			bUseUnity = true;
			MinFilesUsingPrecompiledHeaderOverride = 1;
			bFasterWithoutUnity = false;
		}
		else
		{
			bUseUnity = false;
		}
	}
}

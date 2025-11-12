// CarGameTarget.cs
// Build target for game (standalone/packaged builds)
// Copyright 2025. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CarGameTarget : TargetRules
{
	public CarGameTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "CarGame" });

		// Performance optimizations for shipping
		if (Configuration == UnrealTargetConfiguration.Shipping)
		{
			bUseLoggingInShipping = false;
			bUseChecksInShipping = false;
			bCompileICU = true;
		}
	}
}

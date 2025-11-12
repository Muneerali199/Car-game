// CarGameEditorTarget.cs
// Build target for editor builds
// Copyright 2025. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CarGameEditorTarget : TargetRules
{
	public CarGameEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "CarGame" });

		// Editor-specific settings
		bBuildDeveloperTools = true;
		bBuildWithEditorOnlyData = true;
		bCompileAgainstEngine = true;
		bCompileAgainstCoreUObject = true;
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class BloxorzTarget : TargetRules
{
	public BloxorzTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        bOverrideBuildEnvironment = true;
        ExtraModuleNames.Add("Bloxorz");
	}
}

// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class H1 : ModuleRules
{
	public H1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}

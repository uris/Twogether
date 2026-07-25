// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Together : ModuleRules
{
	public Together(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"OnlineSubsystem",
			"OnlineSubsystemSteam",
			"GameplayTags",
			"CommonUI",
			"CommonInput",
			"PropertyPath"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "AITestSuite" });

		PublicIncludePaths.AddRange(new string[]
		{
			"Together",
			"Together/Variant_Platforming",
			"Together/Variant_Platforming/Animation",
			"Together/Variant_Combat",
			"Together/Variant_Combat/AI",
			"Together/Variant_Combat/Animation",
			"Together/Variant_Combat/Gameplay",
			"Together/Variant_Combat/Interfaces",
			"Together/Variant_Combat/UI",
			"Together/Variant_SideScrolling",
			"Together/Variant_SideScrolling/AI",
			"Together/Variant_SideScrolling/Gameplay",
			"Together/Variant_SideScrolling/Interfaces",
			"Together/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

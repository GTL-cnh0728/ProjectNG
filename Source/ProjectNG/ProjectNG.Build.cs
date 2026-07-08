// Copyright (c) 2025 TeamNG. All Rights Reserved.

using UnrealBuildTool;

public class ProjectNG : ModuleRules
{
	public ProjectNG(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",

			// Input 관련 모듈
			"InputCore",
			"EnhancedInput",

			// GAS 관련 모듈
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			
			// ProjectSetting 모듈
			"DeveloperSettings",
			
			// Slate/SlateCore 모듈
			"Slate",
			 "SlateCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// GAS 관련 모듈
			"GameplayAbilitiesEditor",
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}

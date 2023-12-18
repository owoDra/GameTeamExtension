// Copyright (C) 2023 owoDra

using UnrealBuildTool;

public class GTExt : ModuleRules
{
	public GTExt(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                ModuleDirectory,
                ModuleDirectory + "/GTExt",
            }
        );


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "GameplayTags",
                "ModularGameplay",
                "Niagara",
                "AIModule",
                "NetCore",
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "GFCore",
            }
        );
    }
}

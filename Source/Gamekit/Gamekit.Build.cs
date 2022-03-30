// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

using UnrealBuildTool;

public class Gamekit : ModuleRules
{
    public Gamekit(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[]{
        });

        PublicDependencyModuleNames.AddRange(new string[]{
            "Core",
            "RenderCore",
            "RHI"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            // Gameplay Abilities
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",

            "Renderer",
            "Projects",
            // Used for ads
            "HTTP",
            "Json",
            "JsonUtilities",
            // Defaults
            "CoreUObject",
            "Engine",
            "MoviePlayer",
            "InputCore",
            "HeadMountedDisplay",
            "NavigationSystem",
            "AIModule",
            "UMG",
            "Slate",
            "SlateCore",
            // Particles
            "Niagara",
            //
            "EnhancedInput",
            "Landscape",
        });
    }
}

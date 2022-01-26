// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

using UnrealBuildTool;

public class Gamekit : ModuleRules
{
    public Gamekit(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[]{
            "Gamekit",
        });

        //PublicIncludePaths.AddRange(new string[]{
        //    "Gamekit",
        //});

        PublicDependencyModuleNames.AddRange(new string[]{
             // Gameplay Abilities
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            // Custom Shader Location
            "RHI",
            "RenderCore",
            // Used for ads
            "HTTP",
            "Json",
            "JsonUtilities",
            // Defaults 
            "Core",
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

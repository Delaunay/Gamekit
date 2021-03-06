// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

using UnrealBuildTool;

public class Gamekit : ModuleRules
{
    public Gamekit(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[]{
            "Gamekit"
        });

        PublicDependencyModuleNames.AddRange(new string[]{
            "NetCore",
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

            // Matchmaking
            "OnlineSubsystem",
            "OnlineSubsystemUtils",

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

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("EditorFramework");
            PrivateDependencyModuleNames.Add("UnrealEd");
            PrivateDependencyModuleNames.Add("Slate");
            PrivateDependencyModuleNames.Add("SequenceRecorder");
        }
    }
}

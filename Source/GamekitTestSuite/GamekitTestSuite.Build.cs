// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

using System.IO;
using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class GamekitTestSuite : ModuleRules
    {
        public GamekitTestSuite(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(new string[] {
               
            });

            PublicDependencyModuleNames.AddRange(new string[] {
                "Core",
                "RenderCore",
                "RHI",

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

                // Our plugin we want to test
                "Gamekit",
            });

            if (Target.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
            }
        }
    }
}
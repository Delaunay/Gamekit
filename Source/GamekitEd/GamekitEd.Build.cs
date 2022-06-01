// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

using UnrealBuildTool;

public class GamekitEd : ModuleRules
{
    public GamekitEd(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[]{
            "GamekitEd"
        });

        PublicDependencyModuleNames.AddRange(new string[]{
            "UnrealEd",

            "Gamekit",
            "NetCore",
            "Core",
            "RenderCore",
            "RHI"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "GameplayAbilities",

            // Defaults
            "CoreUObject",
        });
    }
}

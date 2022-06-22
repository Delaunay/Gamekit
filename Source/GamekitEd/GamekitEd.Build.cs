// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

using UnrealBuildTool;

public class GamekitEd : ModuleRules
{
    public GamekitEd(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.AddRange(new string[]{
            "GamekitEd",

            "Editor/PropertyEditor/Private"
        });

        PublicDependencyModuleNames.AddRange(new string[]{
            "Gamekit",

            "Projects",
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "Slate",
            "SlateCore",
            "PropertyEditor",
            "LevelEditor",

            "GameplayAbilities",
            "PythonScriptPlugin",
            "AssetTools",
            "EditorScriptingUtilities"
        });

        /*
        PrivateIncludePathModuleNames.AddRange(new string[] {
            "UnrealEd",
        });

        
        PrivateDependencyModuleNames.AddRange(new string[] {
            "UnrealEd",
        });
        */
    }
}

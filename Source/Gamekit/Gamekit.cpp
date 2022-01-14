// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#include "Gamekit.h"
#include "Modules/ModuleManager.h"

#include "Engine/Blueprint.h"

DEFINE_LOG_CATEGORY(LogGamekit)

#define GAMEKIT_SHADERS "/Gamekit"

void FGamekitModule::StartupModule() {
	// GamekitShaders
	FString ShaderDirectory = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("Gamekit"), TEXT("Shaders"));

	// Make sure the mapping does not exist before adding it
	if (!AllShaderSourceDirectoryMappings().Contains(GAMEKIT_SHADERS)){
		AddShaderSourceDirectoryMapping(GAMEKIT_SHADERS, ShaderDirectory);
	}
}

void FGamekitModule::ShutdownModule() {
	ResetAllShaderSourceDirectoryMappings();
}

IMPLEMENT_GAME_MODULE(FGamekitModule, Gamekit);

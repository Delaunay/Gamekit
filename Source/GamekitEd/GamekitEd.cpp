// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "GamekitEd.h"
#include "Modules/ModuleManager.h"

#include "Engine/Blueprint.h"

DEFINE_LOG_CATEGORY(LogGamekitEd)


void FGamekitEdModule::StartupModule()
{
}

void FGamekitEdModule::ShutdownModule() { }

IMPLEMENT_GAME_MODULE(FGamekitEdModule, GamekitEd);

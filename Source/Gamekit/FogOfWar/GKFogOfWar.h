// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit/GKLog.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGKFoW, Log, All);

#define GKFOG_WARNING(Format, ...) GAMEKIT_LOG(LogGKFoW, Warning, Format, __VA_ARGS__)
#define GKFOG_DISPLAY(Format, ...) GAMEKIT_LOG(LogGKFoW, Display, Format, __VA_ARGS__)
#define GKFOG_LOG(Format, ...)     GAMEKIT_LOG(LogGKFoW, Log, Format, __VA_ARGS__)

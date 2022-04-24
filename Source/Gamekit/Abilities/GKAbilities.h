// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit/GKLog.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGKGA, Log, All);

#define GKGA_WARNING(Format, ...) GAMEKIT_LOG(LogGKGA, Warning, Format, __VA_ARGS__)
#define GKGA_DISPLAY(Format, ...) GAMEKIT_LOG(LogGKGA, Display, Format, __VA_ARGS__)
#define GKGA_LOG(Format, ...)     GAMEKIT_LOG(LogGKGA, Log, Format, __VA_ARGS__)

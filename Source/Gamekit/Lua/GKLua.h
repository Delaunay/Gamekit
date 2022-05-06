// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit/GKLog.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGKLua, Log, All);

#define GKLUA_WARNING(Format, ...) GAMEKIT_LOG(LogGKLua, Warning, Format, __VA_ARGS__)
#define GKLUA_DISPLAY(Format, ...) GAMEKIT_LOG(LogGKLua, Display, Format, __VA_ARGS__)
#define GKLUA_LOG(Format, ...)     GAMEKIT_LOG(LogGKLua, Log, Format, __VA_ARGS__)

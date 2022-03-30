// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Experimental/GKTurnGameState.h"

#include "Gamekit/GKSharedTexture.h"

AGKTurnGameState::AGKTurnGameState() { CollisionMap = CreateDefaultSubobject<UGKSharedTexture>(TEXT("CollisionMap")); }
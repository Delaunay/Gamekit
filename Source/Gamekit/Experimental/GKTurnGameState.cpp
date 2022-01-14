// BSD 3-Clause License
//
// Copyright (c) 2019, Pierre Delaunay
// All rights reserved.

#include "GKTurnGameState.h"
#include "GKSharedTexture.h"

AGKTurnGameState::AGKTurnGameState()
{
    CollisionMap = CreateDefaultSubobject<UGKSharedTexture>(TEXT("CollisionMap"));
}
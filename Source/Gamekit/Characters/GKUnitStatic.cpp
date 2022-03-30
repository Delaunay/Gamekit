// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Characters/GKUnitStatic.h"

FGKUnitStatic::FGKUnitStatic(): Health(100.0f), HealthRegen(1.0f), Mana(100.0f), ManaRegen(1.0f)
{
    AttackRate           = 1;
    AttackAnimationPoint = 1.f;
    AttackRange          = 300.f;
    Damage               = 50;
    MovementSpeed        = 300;
    Icon                 = nullptr;
}

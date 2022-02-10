#include "Projectiles/GKProjectileStatic.h"


FGKProjectileStatic::FGKProjectileStatic()
{
    BaseSpeed              = 1800;
    InitialSpeed       = 1800;
    HomingAcceleration = 16000;

	// Sane defaults so people experimenting will get something that works
    Range    = 3200;
    Behavior = EGK_ProjectileBehavior::Directional;
}

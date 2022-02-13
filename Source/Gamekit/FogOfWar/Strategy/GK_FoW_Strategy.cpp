#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"

#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"


void UGKFogOfWarStrategy::DrawFactionFog() {
	for (auto &Component: FogOfWarVolume->ActorComponents)
    {
        DrawLineOfSight(Component);
    }
}
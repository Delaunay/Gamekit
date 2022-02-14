#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"

#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"


UGKFogOfWarStrategy::UGKFogOfWarStrategy() {

}

void UGKFogOfWarStrategy::DrawFactionFog() {
	for (auto &Component: FogOfWarVolume->ActorComponents)
    {
        DrawLineOfSight(Component);
    }
}

void UGKFogOfWarStrategy::Initialize() { 
	FogOfWarVolume = Cast<AGKFogOfWarVolume>(GetOwner());
}

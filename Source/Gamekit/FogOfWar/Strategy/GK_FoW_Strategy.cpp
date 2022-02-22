#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"

#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"


UGKFogOfWarStrategy::UGKFogOfWarStrategy() {

}

void UGKFogOfWarStrategy::DrawFactionFog() {
    TSet<FName> Factions;

	for (auto &Component: FogOfWarVolume->ActorComponents)
    {
        DrawLineOfSight(Component);
        Factions.Add(Component->Faction);
    }

    for (auto Name: Factions)
    {
        FogOfWarVolume->TextureReady(Name);
    }
}

void UGKFogOfWarStrategy::Initialize() { 
	FogOfWarVolume = Cast<AGKFogOfWarVolume>(GetOwner());
}

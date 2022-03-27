#include "FogOfWar/Strategy/GK_FoW_Strategy.h"

#include "FogOfWar/GKFogOfWarVolume.h"
#include "FogOfWar/GKFogOfWarComponent.h"


UGKFogOfWarStrategy::UGKFogOfWarStrategy() {

}

void UGKFogOfWarStrategy::DrawFactionFog(FGKFactionFog *FactionFog)
{
    TSet<FName> Factions;

	for (auto &Component: FactionFog->Allies)
    {
            DrawLineOfSight(FactionFog, Component);
    }
}

void UGKFogOfWarStrategy::Initialize() { 
	FogOfWarVolume = Cast<AGKFogOfWarVolume>(GetOwner());
}


void UGKFogOfWarStrategy::DebugDrawComponent(class UGKFogOfWarComponent *Component)
{
    if (FogOfWarVolume->bDebug)
    {
        auto Location = Component->GetOwner()->GetActorLocation();

        UKismetSystemLibrary::DrawDebugCircle(GetWorld(),            // World
                                              Location,              // Center
                                              Component->Radius,     // Radius
                                              72,                    // NumSegments
                                              FLinearColor::White,   // LineColor
                                              0.f,                   // LifeTime
                                              5.f,                   // Tickness
                                              FVector(1, 0, 0),      // YAxis
                                              FVector(0, 1, 0),      // ZAxis
                                              true                   // DrawAxes
        );
    }
}


void UGKFogOfWarStrategy::DebugDrawPoint(FVector Center, FLinearColor Color, float Radius)
{                                      
    if (FogOfWarVolume->bDebug)                                    
    {                                                              
        UKismetSystemLibrary::DrawDebugCircle(
            GetWorld(),          
            Center,                  
            Radius,                
            36,                  
            Color, 
            0.f,                 
            5.f,                 
            FVector(1, 0, 0),    
            FVector(0, 1, 0),    
            true
        );
    }
}

void UGKFogOfWarStrategy::AddVisibleActor(FGKFactionFog *FactionFog, UGKFogOfWarComponent *SourceComp, AActor *Target)
{
    UActorComponent *     Component = Target->GetComponentByClass(UGKFogOfWarComponent::StaticClass());
    UGKFogOfWarComponent *FoWComp   = Cast<UGKFogOfWarComponent>(Component);

    SourceComp->OnSighting.Broadcast(Target);

    // Avoid multiple broadcast per target
    if (FoWComp != nullptr && !FactionFog->VisibleEnemies.Contains(FoWComp))
    {
        FactionFog->VisibleEnemies.Add(FoWComp);
        FoWComp->OnSighted.Broadcast(SourceComp->GetOwner());
    }
}


void UGKFogOfWarStrategy::AddVisibleComponent(struct FGKFactionFog *      FactionFog,
                                              class UGKFogOfWarComponent *SourceComp,
                                              class UGKFogOfWarComponent *SightedComp)
{
    if (SightedComp == nullptr)
    {
        return;
    }

    SourceComp->OnSighting.Broadcast(SightedComp->GetOwner());

    // Avoid multiple broadcast per target
    if (!FactionFog->VisibleEnemies.Contains(SightedComp))
    {
        FactionFog->VisibleEnemies.Add(SightedComp);
        SightedComp->OnSighted.Broadcast(SourceComp->GetOwner());
    }
}

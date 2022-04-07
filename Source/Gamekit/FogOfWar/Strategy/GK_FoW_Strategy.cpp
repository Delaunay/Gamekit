#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/Utilities/GKBitFlag.h"
#include "Gamekit/GKLog.h"


UGKFogOfWarStrategy::UGKFogOfWarStrategy() {}

void UGKFogOfWarStrategy::DrawFactionFog(class AGKTeamFog *FactionFog)
{
    TSet<FName> Factions;

    for (auto &Component: FactionFog->Allies)
    {
        DrawLineOfSight(FactionFog, Component);
    }
}

void UGKFogOfWarStrategy::Initialize() { FogOfWarVolume = Cast<AGKFogOfWarVolume>(GetOwner()); }

void UGKFogOfWarStrategy::DebugDrawComponent(class UGKFogOfWarComponent *Component)
{
    if (FogOfWarVolume->bDebug)
    {
        auto Location = Component->GetOwner()->GetActorLocation();

        UKismetSystemLibrary::DrawDebugCircle(GetWorld(),          // World
                                              Location,            // Center
                                              Component->Radius,   // Radius
                                              72,                  // NumSegments
                                              FLinearColor::White, // LineColor
                                              0.f,                 // LifeTime
                                              5.f,                 // Tickness
                                              FVector(1, 0, 0),    // YAxis
                                              FVector(0, 1, 0),    // ZAxis
                                              true                 // DrawAxes
        );
    }
}

void UGKFogOfWarStrategy::DebugDrawPoint(FVector Center, FLinearColor Color, float Radius)
{
    if (FogOfWarVolume->bDebug)
    {
        UKismetSystemLibrary::DrawDebugCircle(
                GetWorld(), Center, Radius, 36, Color, 0.f, 5.f, FVector(1, 0, 0), FVector(0, 1, 0), true);
    }
}

void UGKFogOfWarStrategy::AddVisibleActor(class AGKTeamFog *FactionFog, UGKFogOfWarComponent *SourceComp, AActor *Target)
{
    UActorComponent *     Component = Target->GetComponentByClass(UGKFogOfWarComponent::StaticClass());
    UGKFogOfWarComponent *SightedComp = Cast<UGKFogOfWarComponent>(Component);

    AddVisibleComponent(FactionFog, SourceComp, SightedComp);
}

void UGKFogOfWarStrategy::AddVisibleComponent(class AGKTeamFog *      FactionFog,
                                              class UGKFogOfWarComponent *SourceComp,
                                              class UGKFogOfWarComponent *SightedComp)
{
    if (SightedComp == nullptr)
    { 
        return;
    }

    // Vision is symetric 
    // SightedComp->TeamVisibility = SetFlag(SightedComp->TeamVisibility, SourceComp->GetGenericTeamId().GetId());
    // SourceComp->TeamVisibility  = SetFlag(SourceComp->TeamVisibility, SightedComp->GetGenericTeamId().GetId());
    
    SourceComp->OnSighting.Broadcast(SightedComp->GetOwner());

    /*
    GK_WARNING(TEXT("Server: %s sees %s (%d)"),
        *AActor::GetDebugName(SourceComp->GetOwner()),
        *AActor::GetDebugName(SightedComp->GetOwner()),
        SightedComp->TeamVisibility);
    */

    // Avoid multiple broadcast per target
    if (!FactionFog->Visible.Contains(SightedComp))
    {
        FactionFog->Visible.Add(SightedComp);
        SightedComp->OnSighted.Broadcast(SourceComp->GetOwner());
    }
}
 
#include "Gamekit/FogOfWar/Strategy/GK_FoW_Strategy.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/Utilities/GKBitFlag.h"


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
    UGKFogOfWarComponent *FoWComp   = Cast<UGKFogOfWarComponent>(Component);

    SourceComp->OnSighting.Broadcast(Target);

    // Avoid multiple broadcast per target
    if (FoWComp != nullptr && !FactionFog->VisibleSoFar.Contains(FoWComp))
    {
        FactionFog->VisibleSoFar.Add(FoWComp);
        FoWComp->OnSighted.Broadcast(SourceComp->GetOwner());
    }
}

void UGKFogOfWarStrategy::AddVisibleComponent(class AGKTeamFog *      FactionFog,
                                              class UGKFogOfWarComponent *SourceComp,
                                              class UGKFogOfWarComponent *SightedComp)
{
    if (SightedComp == nullptr)
    {
        return;
    }

    // FIXME
    // SightedComp->TeamVisibility = SetFlag(SightedComp->TeamVisibility, SourceComp->GeTeamId());
    SourceComp->OnSighting.Broadcast(SightedComp->GetOwner());

    // Avoid multiple broadcast per target
    if (!FactionFog->VisibleSoFar.Contains(SightedComp))
    {
        FactionFog->VisibleSoFar.Add(SightedComp);
        SightedComp->OnSighted.Broadcast(SourceComp->GetOwner());
    }
}
 
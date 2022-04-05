// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/States/GKPrivatePlayerState.h"

// Gamekit
#include "Gamekit/Gamekit.h"
#include "Gamekit/GKLog.h"

// Unreal Engine
#include "Net/UnrealNetwork.h"


AGKPrivatePlayerState::AGKPrivatePlayerState() {
    bOnlyRelevantToOwner  = false;
    bReplicates = true;
    PrimaryActorTick.bCanEverTick = GK_PRIVATEPLAYERSTATE_SANITY;
}

bool AGKPrivatePlayerState::IsNetRelevantFor(
    const AActor * RealViewer,  // Network connection (Player Controller / UPlayer)
    const AActor * ViewTarget,  // Actor which the player is viewing through
    const FVector & SrcLocation) const
{
    bool bRelevant = false;
    auto Attitude = FGenericTeamId::GetAttitude(GetGenericTeamId(), FGenericTeamId::GetTeamIdentifier(RealViewer));
    
    if (Attitude == ETeamAttitude::Friendly)
    {
        bRelevant = true;
    }

    return bRelevant;
}

FGenericTeamId AGKPrivatePlayerState::GetGenericTeamId() const {
    // Owner is the PlayerController
    return FGenericTeamId::GetTeamIdentifier(GetOwner());
}

#if GK_PRIVATEPLAYERSTATE_SANITY
void AGKPrivatePlayerState::Tick(float Delat) {
    //
    if (GetNetMode() == ENetMode::NM_Client)
        return;

    Version += 1;
    // GK_WARNING(TEXT("AGKPrivatePlayerState::Server version is %d"), Version);
}

void AGKPrivatePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const {
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGKPrivatePlayerState, Version);
}

void AGKPrivatePlayerState::OnRep_Version() { 
    // 
    // GK_WARNING(TEXT("AGKPrivatePlayerState::OnRep_Version %d"), Version); 
}
#endif

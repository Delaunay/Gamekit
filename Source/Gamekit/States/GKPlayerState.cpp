// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.


#include "Gamekit/States/GKPlayerState.h"

// Gamekit
#include "Gamekit/Gamekit.h"
#include "Gamekit/GKLog.h"
#include "Gamekit/States/GKPrivatePlayerState.h"

// Unreal Engine
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


AGKPlayerState::AGKPlayerState() {
    bReplicates = true;
    bOnlyRelevantToOwner  = false;
    bAlwaysRelevant = true;
    PrivatePlayerStateClass = AGKPrivatePlayerState::StaticClass();
    PrimaryActorTick.bCanEverTick = GK_PLAYERSTATE_SANITY;
}

void AGKPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGKPlayerState, PrivatePlayerState);

#if GK_PLAYERSTATE_SANITY
    DOREPLIFETIME(AGKPlayerState, Version);
#endif
}

void AGKPlayerState::PreInitializeComponents() {
    if (GetNetMode() == NM_Client)
        return;

    GK_WARNING(TEXT("Spawning PrivatePlayerState"));

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Owner = GetOwner();
    SpawnInfo.Instigator = GetInstigator();
    SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    SpawnInfo.ObjectFlags |= RF_Transient;	// We never want player states to save into a map

    PrivatePlayerState = GetWorld()->SpawnActor<AGKPrivatePlayerState>(PrivatePlayerStateClass, SpawnInfo);
} 

/*
#if GK_PLAYERSTATE_SANITY
void AGKPlayerState::Tick(float Delta) {
    if (GetNetMode() == ENetMode::NM_Client)
        return;

    // GK_WARNING(TEXT("Server version is %d"), Version);

    Version += 1;
    // SetScore(Version);
    // MARK_PROPERTY_DIRTY_FROM_NAME(AGKPlayerState, Version, this);

    // FlushNetDormancy
    // ForceNetUpdate();
}

void AGKPlayerState::OnRep_Version() { 
    //
    // GK_WARNING(TEXT("AGKPlayerState::OnRep_Version %d"), Version); 
}
#endif
*/


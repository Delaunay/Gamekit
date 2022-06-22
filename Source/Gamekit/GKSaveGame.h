// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"
#include "Gamekit/Abilities/GKAbilityInterface.h"

// Unreal Engine
#include "GameFramework/SaveGame.h"

// Generated
#include "GKSaveGame.generated.h"

/** List of versions, native code will handle fixups for any old versions */
namespace EGKSaveGameVersion
{
enum type
{
    // Initial version
    Initial,

    // -----<new versions must be added before this line>-------------------------------------------------
    VersionPlusOne,
    LatestVersion = VersionPlusOne - 1
};
} // namespace EGKSaveGameVersion

/** Object that is written to and read from the save game archive, with a data version */
UCLASS(BlueprintType)
class GAMEKIT_API UGKSaveGame: public USaveGame
{
    GENERATED_BODY()

    public:
    /** Constructor */
    UGKSaveGame()
    {
        // Set to current version, this will get overwritten during serialization when loading
        SavedDataVersion = EGKSaveGameVersion::LatestVersion;
    }

    /** User's unique id */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SaveGame)
    FString UserId;

    protected:
    /** What LatestVersion was when the archive was saved */
    UPROPERTY()
    int32 SavedDataVersion;

    /** Overridden to allow version fixups */
    virtual void Serialize(FArchive &Ar) override;
};

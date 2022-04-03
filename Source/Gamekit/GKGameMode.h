// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "GameFramework/GameModeBase.h"

// Generated
#include "GKGameMode.generated.h"

/**
 *  
 *  Player Team assignment
 * 
 *  #. Method
 * 
 *     #. Team Assignment is done with player start
 *     #. Once assigned player restart reuse the same set of starting point
 *     
 *  #. Method
 * 
 *      #. Team Assignment is done on player join
 *      #. Team Assignment is done my match maker
 * 
 * 
 *  1. Player are assigned to teams
 *  2. Teams are assigned to players
 *
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKGameModeBaseBase: public AGameModeBase
{
    GENERATED_BODY()

    public:
    /** Constructor */
    AGKGameModeBaseBase();

    FString InitNewPlayer(
        APlayerController * NewPlayerController,
        const FUniqueNetIdRepl & UniqueId,
        const FString & Options,
        const FString & Portal) override;

    //! Spawn the default pawn for a given player & set the team assignment
    APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

    //! Set the faction for a given controller
    // void GenericPlayerInitialization(AController* Controller) override; 

    /** Overriding this function is not essential to this sample since this function
     *	is not being called in normal circumstances. Added just to streamline
     *	dev-time activities (like automated testing). The default ResetLevel
     *	implementation doesn't work all that well with how things are set up in
     *	ActionRPG (and that's ok, this is exactly why we override functions!).
     */
    virtual void ResetLevel() override;

    /** Returns true if GameOver() has been called, false otherwise */
    virtual bool HasMatchEnded() const override;

    /** Called when the game is over i.e. the player dies, the time runs out or the
     *	game is finished*/
    UFUNCTION(BlueprintCallable, Category = Game)
    virtual void GameOver();

    protected:
    UFUNCTION(BlueprintImplementableEvent,
              Category = Game,
              meta     = (DisplayName = "DoRestart", ScriptName = "DoRestart"))
    void K2_DoRestart();

    UFUNCTION(BlueprintImplementableEvent,
              Category = Game,
              meta     = (DisplayName = "OnGameOver", ScriptName = "OnGameOver"))
    void K2_OnGameOver();

    UPROPERTY(BlueprintReadOnly, Category = Game)
    uint32 bGameOver : 1;

    void SetGenericTeamIdFromPlayerStart(AController *NewPlayer, AActor *StartSpot);
};

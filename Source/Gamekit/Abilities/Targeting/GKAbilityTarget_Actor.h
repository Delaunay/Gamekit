// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.
#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityStatic.h"

// Unreal Engine
#include "Abilities/GameplayAbilityTargetActor.h"

// Generated
#include "GKAbilityTarget_Actor.generated.h"

class APlayerController;
class UAbilitySystemComponent;
class UGameplayAbility;

/**
 * It will replace ``AGameplayAbilityTargetActor`` fully when refactoring is done
 * This has a goal of making the AbilityTargetActor fully reusable
 */
UCLASS(Blueprintable)
class GAMEKIT_API AGKAbilityTarget_Actor: public AGameplayAbilityTargetActor
{
    GENERATED_UCLASS_BODY()

    public:
    // deprecate this
    virtual void StartTargeting(class UGameplayAbility *Ability) override final;

    //! Enable the targeting actor
    virtual void StartTargeting(class UGKGameplayAbility *Ability);

    virtual void InitializeFromAbilityData(FGKAbilityStatic const &AbilityData);

    //! Call this to initialize the AbilityTarget actor with the Ability spec we are targeting for
    UFUNCTION(BlueprintImplementableEvent,
              DisplayName = "InitializeFromAbilityData",
              meta        = (ScriptName = "InitializeFromAbilityData"))
    void K2_InitializeFromAbilityData(FGKAbilityStatic const &AbilityData);

    //! Enable use input
    virtual void EnableUserInput();

    //! Disable the targeting actor / make it ready for reuse
    virtual void StopTargeting();

    // UPROPERTY()
    // UGameplayAbility* OwningAbility;

    //! Derived from OwningAbility when possible
    UPROPERTY(BlueprintReadOnly)
    class UAbilitySystemComponent *AbilitySystemComponent;

    UPROPERTY(BlueprintReadWrite);
    bool bTickEnabled;

    public:
    //! Override AGameplayAbilityTargetActor::EndPlay so it is never called
    void EndPlay(const EEndPlayReason::Type EndPlayReason) final;
};

// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "Abilities/GameplayAbilityTypes.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

// Generated
#include "GKQuestComponent.generated.h"

/*
 * Simple component that tracks open quests (Gameplay Tags)
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEKIT_API UGKQuestComponent: public UActorComponent
{
    GENERATED_BODY()

    public:
    // Sets default values for this component's properties
    UGKQuestComponent();

    protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    public:
    // Called every frame
    virtual void TickComponent(float                        DeltaTime,
                               ELevelTick                   TickType,
                               FActorComponentTickFunction *ThisTickFunction) override;

    void GiveQuest(FName QuestName);

    void OnGameplayQuestEvent(FGameplayTag Tag, const FGameplayEventData *Payload);

    // TArray<> ActiveQuests;
    // TArray<> CompletedQuests;

    //! Datatable used to fetch quest data
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Quest")
    class UDataTable *GlobalQuestTable;

    UAbilitySystemComponent *AbilitySystemComponent;
};

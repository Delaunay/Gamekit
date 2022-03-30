// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"

// Generated
#include "GKQuestData.generated.h"

// List of objective that can be done in any order
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKSimpleObjective
{
    GENERATED_USTRUCT_BODY()

    public:
    //! The quest tag that is sent when the objective is finished
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest)
    FGameplayTagContainer ObjectiveTags;

    //! Localized description of the objective
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest)
    FText ShortDescription;

    //! Number of time this objective needs to be completed to be marked as finished
    //! i.e number of stuff you need to kill
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest)
    float Count;
};

// List of objective that can be done in any order
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKQuestObjectives
{
    GENERATED_USTRUCT_BODY()

    public:
    //! List of sub-objectives to finish before this objective can be considered finished
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest)
    TArray<FGKSimpleObjective> Objectives;

    //! Localized description of the objective
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest)
    FText ShortDescription;

    //! Force objective to be completed sequentially
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest)
    bool Sequential;
};

/*!
 * Quest Types
 *	#. Kill
 *		#. Kill the right type of enemies
 *
 *  #. Protect Target
 *		#. Kill enemies without you or the target dying
 *			Variation of the same thing:
 *
 *			#. Escort: protect moving target
 *			#. Rescue: i.e Defense & Escort
 *			#. Defense: protect static target
 *
 *  #. Delivery
 *		Receive Item & deliver it to the target
 *
 *  #. Gather
 *		#. Gather Items
 *		#. Gather Evidence
 *
 *  #. Combo (requires the player to do something specifc)
 *		#. State machine that tracks the player moves
 *
 *  #. Stealth
 *		#. Do not get detected
 *
 *  #. Timed
 *		#. Finish all the objective on time
 *
 *  #. Race
 *		#. Cross a finish line first (might have sub goals to avoid players from making their own track)
 *
 * Other Features:
 *  #. Quest Chains
 *  #. Quest Path  Guide to the overall location of the quest objective
 *  #. Quest Area  Highlight the overall area of the quest
 *
 */
USTRUCT(BlueprintType)
struct GAMEKIT_API FGKQuestData: public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    public:
    FGKQuestData() {}

    //! Primary key not visible
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest)
    FName Name;

    //! Localized name for display
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest)
    FText QuestName;

    //! Localized description for display
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest)
    FText QuestDescription;

    //! Objective are stored as Game tags
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Objective)
    TArray<FGKQuestObjectives> Objectives;
};

UCLASS(BlueprintType)
class UGKQuestDataRef: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest, Meta = (ExposeOnSpawn = true))
    FGKQuestData Data;
};

UCLASS(BlueprintType)
class UGKQuestObjectiveRef: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest, Meta = (ExposeOnSpawn = true))
    FGKQuestObjectives Data;
};

UCLASS(BlueprintType)
class UGKQuestSimpleObjectiveRef: public UObject
{
    GENERATED_BODY()
    public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Quest, Meta = (ExposeOnSpawn = true))
    FGKSimpleObjective Data;
};

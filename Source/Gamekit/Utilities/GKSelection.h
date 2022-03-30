// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "Components/ActorComponent.h"
#include "Containers/Array.h"
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Math/Box.h"
#include "Math/Vector.h"

// Generated
#include "GKSelection.generated.h"

USTRUCT(BlueprintType)
struct GAMEKIT_API FGKUnitGroup
{
    GENERATED_USTRUCT_BODY()

    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<AActor *> Units;
};

/* Select a group of actors that is inside a box.
 * The box bound is given by the cursor.
 *
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GAMEKIT_API UGKBoxSelectionComponent: public UActorComponent
{
    GENERATED_BODY()

    public:
    UGKBoxSelectionComponent();

    //! Start a box selection using the cursor position as the starting point
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void StartBoxSelection(class APlayerController *Controller, ETraceTypeQuery TraceChannel);

    //! Start the selection box if not current selecting
    //! Update the selection box if currently selecting
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void StartOrUpdateBoxSelection(class APlayerController *Controller, ETraceTypeQuery TraceChannel);

    //! Update the box selection using the cursor position as the end point
    UFUNCTION(BlueprintCallable, Category = "Selection")
    void UpdateBoxSelection(class APlayerController *Controller);

    //! Returns the list of selected pawn given the current selection box
    UFUNCTION(BlueprintCallable, Category = "Selection", meta = (WorldContext = "World"))
    void FetchBoxSelection(const UObject *World, TArray<AActor *> &Out);

    //! Stop selection and returns the last selection
    UFUNCTION(BlueprintCallable, Category = "Selection", meta = (WorldContext = "World"))
    void EndBoxSelection(const UObject *World);

    //! Draw the current selction box for debug purposes
    UFUNCTION(BlueprintCallable, Category = "Debug|Selection", meta = (WorldContext = "World"))
    void DrawBoxSelection(const UObject *World);

    //! Get the center of the selection
    UFUNCTION(BlueprintPure, Category = "Selection")
    FVector GetCenter();

    //! Get the extent of the selection
    UFUNCTION(BlueprintPure, Category = "Selection")
    FVector GetExtent();

    //! Get the size of the selection
    UFUNCTION(BlueprintPure, Category = "Selection")
    FVector GetSize();

    //! Last hit result
    UPROPERTY(BlueprintReadOnly)
    FHitResult HitResult;

    //! Starting position of the cursor
    UPROPERTY(BlueprintReadOnly)
    FVector Start;

    //! Latest registered position of the cursor
    UPROPERTY(BlueprintReadOnly)
    FVector End;

    //! Selection box, only valid when Selecting is true
    UPROPERTY(BlueprintReadOnly)
    FBox Box;

    //! Set to true when currently selecting
    UPROPERTY(BlueprintReadOnly)
    bool Selecting;

    //! Increase the Extent size by a given amount
    //! Useful to make sure you are selecting everything
    //! Regardless of its altitude
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector ExtentMargin;

    //! Trace channel used to find the world position of the cursor
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TEnumAsByte<ETraceTypeQuery> TraceChannel;

    //! Object type we want to select
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

    //! Object class we want to select
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UClass *ActorClassFilter;

    //! Specific actors we would like to ignore
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<AActor *> ActorsToIgnore;
};

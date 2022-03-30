// BSD 3-Clause License
//
// Copyright (c) 2022, Pierre Delaunay
// All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// Generated
#include "GKMovementUtility.generated.h"

/**
 *
 */
UCLASS()
class GAMEKIT_API UGKMovementUtility: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    public:
    // Same as SimpleMoveToLocation but does not stop if we are close enough
    UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
    static void SimpleMoveToLocationExact(class AController *Controller, const FVector &Goal, float AcceptanceRadius);

    // Do a simple inplace rotation on the Yaw axis, returns false if we have reached the desired rotation
    // This needs to be called every tick until it returns false
    // I would like to add it to some MovementComponent but the component is fairly complex
    // and it looks like it relies on velocity to know if it needs to move
    // which might be problematic for in-place rotation like this
    // I might have to reword my movement code to use the ability system instead
    UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
    static bool SimpleYawTurn(class UCharacterMovementComponent *Movement,
                              FRotator                           DesiredRotation,
                              float                              DeltaTime,
                              FRotator &                         FinalRotation);
};

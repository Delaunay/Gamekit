// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilityInputs.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameFramework/PlayerInput.h"

// Generated
#include "GKInputLibrary.generated.h"

/**
 *
 */
UCLASS()
class GAMEKIT_API UGKInputLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintPure, Category = "Input")
    static FInputActionValue MakeBooleanAction(bool value);

    UFUNCTION(BlueprintPure, Category = "Input")
    static FInputActionValue MakeAxis1DAction(float value);

    UFUNCTION(BlueprintPure, Category = "Input")
    static FInputActionValue MakeAxis2DAction(FVector2D value);

    UFUNCTION(BlueprintPure, Category = "Input")
    static FInputActionValue MakeAxis3DAction(FVector value);

    UFUNCTION(BlueprintPure, Category = "Input|EdgePan")
    static bool IsEdgePanning(FVector2D MouseXYViewport, FVector4 Margins);

    UFUNCTION(BlueprintPure, Category = "Input|EdgePan")
    static FVector2D EdgePanDirection(FVector2D MouseXYViewport);

    //! Fetch a lost of Keys matching a given action name
    UFUNCTION(BlueprintPure, Category = "Input")
    static const TArray<FInputActionKeyMapping>& GetKeysForAction(APlayerController* Controller, const FName& ActionName);

    //! Same as `GetKeysFromInputEnum` but assume `EGK_MOBA_AbilityInputID` as its input enum
    UFUNCTION(BlueprintPure, Category = "Input")
    static FText GetKeysForInputID(APlayerController* Controller, const EGK_MOBA_AbilityInputID InputID);

    /*!Returns the Name of the key binded to a given Input Slot
     * 
     * \rst
     * 
     * Examples
     * --------
     * 
     * .. code-block:: cpp
     * 
     *      FText KeyName = GetKeysFromInputEnum(
     *          Controller,
     *          StaticEnum<EGK_MOBA_AbilityInputID>(),
     *          int(EGK_MOBA_AbilityInputID::Skill1)
     *      );
     * 
     *      Print(KeyName); // "Q"
     * 
     * \endrst
     */
    UFUNCTION(BlueprintPure, Category = "Input")
    static FText GetKeysFromInputEnum(APlayerController* Controller, UEnum* Enum, int Value);
};

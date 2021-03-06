// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"

// Generated
#include "GKEnhancedInputInterface.generated.h"

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UGKPawnEnhancedInputInterface: public UInterface
{
    GENERATED_BODY()
};

// Using the interface on our Pawn is not working :/
// No logs are printed
class GAMEKIT_API IGKPawnEnhancedInputInterface
{
    public:
    GENERATED_BODY()

    public:
    // Setup the Enhanced Input system, add the default mapping context if set
    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    virtual void InitializeEnhancedInput(class AController *MyController, class UInputComponent *PlayerInputComponent);

    // Add an input context to existing InputSubsystem
    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    virtual void AddMappingContext(class UInputMappingContext *Input, int32 Priority);

    // Remove an input context from the InputSubsystem
    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    virtual void RemoveMappingContext(class UInputMappingContext *Input);

    //! Register an action override
    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    virtual TMap<class UInputAction *, FName> const &GetActionNameOverrides();

    // if enhanced input system enabled will push the input mapping
    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    virtual class UInputMappingContext *GetDefaultInputMapping();

    protected:
    // Cached pointer, populated after `SetupPlayerInputComponent`
    class UEnhancedInputLocalPlayerSubsystem *InputSubsystem;
    class UEnhancedInputComponent *           PlayerEnhancedInputComponent;
};

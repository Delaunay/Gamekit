// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Characters/GKEnhancedInputInterface.h"

// Unreal Engine
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputTriggers.h"
#include "Engine/LocalPlayer.h"


void IGKPawnEnhancedInputInterface::InitializeEnhancedInput(class AController *    MyController,
                                                            class UInputComponent *PlayerInputComponent)
{
    // Cache for later
    PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

    if (PlayerEnhancedInputComponent == nullptr)
    {
        UE_LOG(LogGamekit, Log, TEXT("SetupPlayerInputComponent: Pawn is not using EnhancedInputComponent"));
        return;
    }

    // Using Input Mapping Context
    // ---------------------------

    // Make sure that we have a valid PlayerController.
    if (APlayerController *PC = Cast<APlayerController>(MyController))
    {
        // Get the Enhanced Input Local Player Subsystem from the Local Player related to our
        // Player Controller.
        InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

        if (InputSubsystem)
        {
            // PawnClientRestart can run more than once in an Actor's lifetime, so start by
            // clearing out any leftover mappings.
            InputSubsystem->ClearAllMappings();

            // Add each mapping context, along with their priority values.
            // Higher values outprioritize lower values.
            auto DefaultMapping = GetDefaultInputMapping();
            if (DefaultMapping)
            {
                AddMappingContext(DefaultMapping, 0);
            }
        }
        else
        {
            UE_LOG(LogGamekit, Log, TEXT("SetupPlayerInputComponent: InputSubsystem is null"));
        }
    }
    else
    {
        UE_LOG(LogGamekit, Log, TEXT("SetupPlayerInputComponent: APlayerController is null"));
    }
}

void IGKPawnEnhancedInputInterface::AddMappingContext(class UInputMappingContext *Input, int32 Priority)
{
    if (PlayerEnhancedInputComponent == nullptr)
    {
        UE_LOG(LogGamekit, Log, TEXT("AddMappingContext: Pawn is not using EnhancedInputComponent"));
        return;
    }

    if (Input == nullptr)
    {
        UE_LOG(LogGamekit, Log, TEXT("AddMappingContext: UInputMappingContext is null"));
        return;
    }

    if (InputSubsystem == nullptr)
    {
        UE_LOG(LogGamekit, Log, TEXT("AddMappingContext: UEnhancedInputLocalPlayerSubsystem is null"));
        return;
    }

    auto InputNameOverride = GetActionNameOverrides();

    // Register the Actions
    for (auto &Mapping: Input->GetMappings())
    {
        UInputAction const *Action     = Mapping.Action;
        FName *             ActionName = InputNameOverride.Find(Action);

        if (Action)
        {
            PlayerEnhancedInputComponent->BindAction(
                    Action,                   // const UInputAction* Action
                    ETriggerEvent::Triggered, // ETriggerEvent TriggerEvent
                    (UObject *)this,          // We need a cast here to use the non templated version
                    ActionName ? *ActionName : Action->GetFName() // * ActionName
            );
        }
        else
        {
            UE_LOG(LogGamekit, Warning, TEXT("AddMappingContext: Found UInputAction == null"));
        }
    }

    // Add to the subsystem
    InputSubsystem->AddMappingContext(Input, Priority);
}

void IGKPawnEnhancedInputInterface::RemoveMappingContext(class UInputMappingContext *Input)
{
    if (PlayerEnhancedInputComponent == nullptr)
    {
        UE_LOG(LogGamekit, Log, TEXT("RemoveMappingContext: Pawn is not using EnhancedInputComponent"));
        return;
    }

    if (Input == nullptr)
    {
        UE_LOG(LogGamekit, Log, TEXT("RemoveMappingContext: UInputMappingContext is null"));
        return;
    }

    if (InputSubsystem == nullptr)
    {
        UE_LOG(LogGamekit, Log, TEXT("RemoveMappingContext: UEnhancedInputLocalPlayerSubsystem is null"));
        return;
    }

    // Do I have to remove the callbacks ?

    // Remove from the context
    InputSubsystem->RemoveMappingContext(Input);
}

TMap<class UInputAction *, FName> const &IGKPawnEnhancedInputInterface::GetActionNameOverrides()
{
    static TMap<class UInputAction *, FName> empty;
    return empty;
}

UInputMappingContext *IGKPawnEnhancedInputInterface::GetDefaultInputMapping() { return nullptr; }
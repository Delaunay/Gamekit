// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GKEnhancedInputInterface.h"
#include "GKPawnEnchancedInput.generated.h"


UCLASS()
class GAMEKIT_API AGKPawnEnchancedInput: public APawn //, public IGKPawnEnhancedInputInterface 
{
    public:
    GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGKPawnEnchancedInput();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	// Enhanced Input Interface
	// ------------------------

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    TMap<class UInputAction *, FName> const &GetActionNameOverrides();

    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    class UInputMappingContext *GetDefaultInputMapping();

	UPROPERTY(EditAnywhere)
	class UInputMappingContext *      DefaultInputMapping;
       
	UPROPERTY(EditAnywhere)
	TMap<class UInputAction *, FName> InputNames;

    //*
	// Setup the Enhanced Input system, add the default mapping context if set
    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    void InitializeEnhancedInput(class AController *    MyController,
                                 class UInputComponent *PlayerInputComponent);

    // Add an input context to existing InputSubsystem
    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    void AddMappingContext(class UInputMappingContext *Input, int32 Priority);

    // Remove an input context from the InputSubsystem
    UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Input")
    void RemoveMappingContext(class UInputMappingContext *Input);

    // Cached pointer, populated after `SetupPlayerInputComponent`
    class UEnhancedInputLocalPlayerSubsystem *InputSubsystem;
    class UEnhancedInputComponent *           PlayerEnhancedInputComponent; 
    // */
 };




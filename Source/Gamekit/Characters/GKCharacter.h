// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"
#include "GameFramework/Character.h"
#include "UObject/ScriptInterface.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"

#include "Characters/GKUnitStatic.h"
#include "Items/GKInventoryInterface.h"
#include "Abilities/GKAbilitySystemComponent.h"
#include "Abilities/GKAttributeSet.h"

#include "GKCharacter.generated.h"

class UGKGameplayAbility;
class UGameplayEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGiveAbilityEventSignature, FGameplayAbilitySpec, AbilitySpec);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGKCharacterDiedDelegate, AGKCharacterBase *, Character);


/** Base class for Character, Designed to be blueprinted

Automatic input binding

.. code-block:: cpp

	AbilitySystemComponent->BindAbilityActivationToInputComponent(
		InputComponent,
		FGameplayAbilityInputBinds(
			FString("ConfirmTarget"),							// InConfirmTargetCommand---+
			FString("CancelTarget"),							// InCancelTargetCommand ---+--+
			FString("EGDAbilityInputID"),						// InEnumName				|  |
			static_cast<int32>(EGKAbilityInputID::Confirm),		// InConfirmTargetInputID <-+  |
			static_cast<int32>(EGKAbilityInputID::Cancel)		// InCancelTargetInputID  <----+
		)
	);

*/
UCLASS(Blueprintable)
class GAMEKIT_API AGKCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	AGKCharacterBase();

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	virtual void OnRep_Controller() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Implement IAbilitySystemInterface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterLevel() const;

	/** Modifies the character level, this may change abilities. Returns true on success */
	UFUNCTION(BlueprintCallable)
	virtual bool SetCharacterLevel(int32 NewLevel);

	virtual void OnHealthChanged_Native(const FOnAttributeChangeData &Data);

	void ClearGameplayAbilities();

	UFUNCTION(BlueprintPure, Category = Death)
    bool IsDead();

	UFUNCTION(BlueprintCallable, Category = "Death")
	virtual void Die();

	UPROPERTY(BlueprintAssignable, Category = "Death")
    FGKCharacterDiedDelegate OnCharacterDied;

protected:
	/** The level of this character, should not be modified directly once it has already spawned */
	UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
	int32 CharacterLevel;

	/** The component used to handle ability system interactions */
	UPROPERTY()
	UGKAbilitySystemComponent* AbilitySystemComponent;

	/** List of attributes modified by the ability system */
	UPROPERTY()
	UGKAttributeSet* AttributeSet;

	/** Required to support AIPerceptionSystem */
	virtual FGenericTeamId GetGenericTeamId() const override;

	// Friended to allow access to handle functions above
	friend class UGKAttributeSet;
	friend class UGKAbilitySystemComponent;

	void AddPassiveEffect(UGameplayEffect* Effect);

	/* Game Play Ability System
	 * --------------------------------------------------------------------------------------- */
public:
	// Bind the abilities to inputs
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	//! Can only be called by the Authority,
	UFUNCTION(BlueprintCallable, Category = Abilities)
	void GrantAbility(FGKAbilitySlot Slot, TSubclassOf<UGKGameplayAbility> AbilityClass);

	UPROPERTY(BlueprintAssignable, Category = Abilities)
	FGiveAbilityEventSignature OnGiveAbility;

	//! Client side register the granted ability
	void OnGiveAbility_Native(FGameplayAbilitySpec& AbilitySpec);

	//! Low level ability activation
    UFUNCTION(BlueprintCallable, Category = Abilities)
    bool ActivateAbility(FGKAbilitySlot Slot);

	UFUNCTION(BlueprintCallable, Category = Abilities)
    void MoveToLocation(FVector loc);

	UFUNCTION(BlueprintCallable, Category = Abilities)
	int AbilityCount() const;

	UFUNCTION(BlueprintCallable, Category = Abilities)
	UGKAttributeSet* GetAttributeSet();

	UFUNCTION(BlueprintCallable, Category = Abilities)
	UGKGameplayAbility* GetAbilityInstance(FGKAbilitySlot Slot);

	UFUNCTION(BlueprintCallable, Category = Abilities)
	FGameplayAbilitySpecHandle GetAbilityHandle(FGKAbilitySlot Slot);

	// DataTable 
	// ---------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit|Data")
	class UDataTable* UnitDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit|Data")
	FName UnitRowName;

	UFUNCTION(BlueprintCallable, Category = "Unit|Data", DisplayName = "GetUnitStatic", meta = (ScriptName = "GetUnitStatic"))
	void K2_GetUnitStatic(FGKUnitStatic& UnitStatic, bool& Valid);

	FGKUnitStatic* GetUnitStatic();

	UFUNCTION()
	void OnDataTableChanged_Native();

	//! Called everytime the Static data is loaded (init & reimport)
	//! Needs to be called after ASC get initialized with its Actor info
	virtual void LoadFromDataTable(FGKUnitStatic& UnitDef);

	//! Cached lookup
	FGKUnitStatic* UnitStatic;

	void PostInitProperties() override;

protected:
	TMap<FGKAbilitySlot, FGameplayAbilitySpec> AbilitySpecs;

	bool InputsBound;
};

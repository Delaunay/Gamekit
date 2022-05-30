// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemComponent.h"
#include "Gamekit/Abilities/GKAttributeSet.h"
#include "Gamekit/Characters/GKSelectableInterface.h"
#include "Gamekit/Characters/GKUnitStatic.h"
#include "Gamekit/Gamekit.h"
#include "Gamekit/Items/GKInventoryInterface.h"

// Unreal Engine
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "UObject/ScriptInterface.h"

// Generated
#include "GKCharacter.generated.h"

class UGKGameplayAbility;
class UGameplayEffect;


/** Base class for Character, Designed to be blueprinted

Automatic input binding

.. code-block:: cpp

        AbilitySystemComponent->BindAbilityActivationToInputComponent(
                InputComponent,
                FGameplayAbilityInputBinds(
                        FString("ConfirmTarget"),							//
InConfirmTargetCommand---+
                        FString("CancelTarget"),							//
InCancelTargetCommand ---+--+
                        FString("EGDAbilityInputID"),						// InEnumName
|  | static_cast<int32>(EGKAbilityInputID::Confirm),		// InConfirmTargetInputID <-+  |
                        static_cast<int32>(EGKAbilityInputID::Cancel)		// InCancelTargetInputID  <----+
                )
        );

*/
UCLASS(Blueprintable)
class GAMEKIT_API AGKCharacterBase: public ACharacter,
                                    public IAbilitySystemInterface,
                                    public IGenericTeamAgentInterface,
                                    public IGKSelectableInterface
{
    GENERATED_BODY()

    public:
    // Constructor and overrides
    AGKCharacterBase();

    bool IsReplicationPausedForConnection(const FNetViewer& ConnectionOwnerNetViewer);

    void OnReplicationPausedChanged(bool bIsReplicationPaused);

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void BeginPlay() override;

    virtual void PossessedBy(AController *NewController) override;

    virtual void UnPossessed() override;

    virtual void OnRep_Controller() override;

    void Tick(float Delta) override;

    // ========================================================================
    //  Dying
    // ------------------------------------------------------------------------
    UFUNCTION(BlueprintPure, Category = Death)
    bool IsDead();

    UFUNCTION(BlueprintCallable, Category = "Death")
    virtual void Die();

    UFUNCTION(BlueprintImplementableEvent, Category = "Death", meta = (DisplayName = "On Death"))
    void ReceiveDeath();

    void OnDeath_Native();
    // ========================================================================
 


    // ========================================================================
    // IGKSelectableInterface
    // ------------------------------------------------------------------------
    UFUNCTION(BlueprintCallable, Category = "UnitSelection")
    void Deselect();

    UFUNCTION(BlueprintCallable, Category = "UnitSelection")
    void Select();

    UFUNCTION(BlueprintImplementableEvent, Category = "UnitSelection")
    void OnSelect();

    UFUNCTION(BlueprintImplementableEvent, Category = "UnitSelection")
    void OnDeselect();
    // ========================================================================

    void PostInitProperties() override;

    // Bind the abilities to inputs
    void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    bool InputsBound;

    // ========================================================================
    // Game Play Ability System
    // ------------------------------------------------------------------------
public:
    UFUNCTION(BlueprintCallable, Category = "Abilities|Move")
    void MoveToLocation(FVector loc);

    UFUNCTION(BlueprintCallable, Category = Abilities)
    UGKAttributeSet* GetAttributeSet();

    //! Can only be called by the Authority,
    UFUNCTION(BlueprintCallable, Category = Abilities)
    void GrantAbility(FGKAbilitySlot Slot, TSubclassOf<UGKGameplayAbility> AbilityClass, int Level = 0);

    //! Remove ability, if currently active, it will be removed once the activation ends
    UFUNCTION(BlueprintCallable, Category = Abilities)
    void RemoveAbility(FGKAbilitySlot Slot);

    UFUNCTION(BlueprintCallable, Category = Abilities)
    void EquipItem(EGK_ItemSlot Slot, TSubclassOf<UGKGameplayAbility> AbilityClass);

    // Allow to rebind abilities
    // void BindInputToAbility(EGK_MOBA_AbilityInputID InputID, FGameplayAbilitySpec AbilitySpec);

    UFUNCTION(BlueprintCallable, Category = Abilities)
    int AbilityCount() const;

    // Slot
    UFUNCTION(BlueprintCallable, Category = Abilities)
    UGKGameplayAbility *GetAbilityInstance(FGKAbilitySlot Slot);

    UFUNCTION(BlueprintCallable, Category = Abilities)
    FGameplayAbilitySpecHandle GetAbilityHandle(FGKAbilitySlot Slot);

    //! Low level ability activation
    UFUNCTION(BlueprintCallable, Category = Abilities)
    bool ActivateAbility(FGKAbilitySlot Slot);

    // Friended to allow access to handle functions above
    friend class UGKAttributeSet;
    friend class UGKAbilitySystemComponent;

    void AddPassiveEffect(UGameplayEffect* Effect);

    // Implement IAbilitySystemInterface
    UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    /** Returns the character level that is passed to the ability system */
    UFUNCTION(BlueprintCallable)
    virtual int32 GetCharacterLevel() const;

    /** Modifies the character level, this may change abilities. Returns true on success */
    UFUNCTION(BlueprintCallable)
    virtual bool SetCharacterLevel(int32 NewLevel);

    UFUNCTION(BlueprintImplementableEvent, Category = "Ability", meta = (DisplayName = "On New Ability"))
    void ReceiveNewAbility(FGKAbilitySlot Slot, FGameplayAbilitySpec NewAbilAbilitySpecity);

    //! Client side register the granted ability
    void OnNewAbility_Native(FGameplayAbilitySpec& AbilitySpec);

    virtual void OnHealthChanged_Native(const FOnAttributeChangeData& Data);

    void ClearGameplayAbilities();
protected:
    TMap<FGKAbilitySlot, FGameplayAbilitySpec> AbilitySpecs;

    /** The level of this character, should not be modified directly once it has already spawned */
    UPROPERTY(EditAnywhere, Replicated, Category = Abilities)
    int32 CharacterLevel;

    /** The component used to handle ability system interactions */
    UPROPERTY()
    UGKAbilitySystemComponent* AbilitySystemComponent;

    /** List of attributes modified by the ability system */
    UPROPERTY()
    UGKAttributeSet* AttributeSet;
    // ========================================================================



    // ========================================================================
    // Data Driven Hooks
    // ------------------------------------------------------------------------
protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit|Data")
    class UDataTable* UnitDataTable;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Unit|Data")
    FName UnitRowName;

    //! Cached lookup
    FGKUnitStatic* UnitStatic;

public:
    UFUNCTION(BlueprintCallable,
        Category = "Unit|Data",
        DisplayName = "GetUnitStatic",
        meta = (ScriptName = "GetUnitStatic"))
    void K2_GetUnitStatic(FGKUnitStatic& UnitStatic, bool& Valid);

    FGKUnitStatic* GetUnitStatic();

    UFUNCTION()
    void OnDataTableChanged_Native();

    //! Called everytime the Static data is loaded (init & reimport)
    //! Needs to be called after ASC get initialized with its Actor info
    virtual void LoadFromDataTable(FGKUnitStatic& UnitDef);

    // ========================================================================


    // ========================================================================
    // IGenericTeamAgentInterface
    // ------------------------------------------------------------------------
protected:
    //! This is set by the Player Controller on possession
    UPROPERTY(replicated, BlueprintReadOnly, Category = "Team", ReplicatedUsing=OnRep_Team)
    FGenericTeamId Team;

public:
    UFUNCTION()
    void OnRep_Team();

    void SetGenericTeamId(const FGenericTeamId &TeamID) override;

    /** Retrieve team identifier in form of FGenericTeamId */
    UFUNCTION(BlueprintPure, Category = "Team")
    FGenericTeamId GetGenericTeamId() const override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Team", meta = (DisplayName = "OnTeamAssigned"))
    void ReceiveTeamAssigned(FGenericTeamId NewTeam);

    // ========================================================================

    bool bPreviousVisibility;
};

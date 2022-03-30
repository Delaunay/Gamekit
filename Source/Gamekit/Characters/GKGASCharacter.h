
#pragma once

// Gamekit
#include "Gamekit/Abilities/GKAttributeSet.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"

// Generated
#include "GKGASCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AGKGASCharacter*, Character);

/**
* Base Character for anything using the Ability Component
*   Minions i.e AI controlled unit will have their ASC inside the AActor while 
*   Heros   i.e Player controlled unit will have their ASC inside their Player State
*
*/
UCLASS()
class GAMEKIT_API AGKGASCharacter : public ACharacter, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AGKGASCharacter();

    //! returns the associated ASC associated with this actor
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

    //! We do not want to create N accessors for all the possible attributes we are goint to have
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    virtual class UGKAttributeSet* GetAttributeSet() const;
    
    // TODO: Make a struct with a the starting default per class
    // so all this can be removed
    // we can add a CharacterName instead and initialize the unit from that
    //! Abilities to be granted once the game starts
    UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
    TArray<TSubclassOf<class UGKGameplayAbility>> DefaultAbilities;
    
    //! Initialize the Character on BeginPlay
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    virtual void InitializeGameplayAbilities();
    
    //! Remove all the granted abilities
    //! TODO: Why is this necessary ?
    //! is the Character reused ?
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    virtual void ClearGameplayAbilities();
    
    //! Returns the Abilities that can be activated by the Player
    UFUNCTION(BlueprintCallable, Category = "Abilities")
    void GetActivatableAbilities(TArray<FGameplayAbilitySpecHandle>& Abilities);

protected:
    class UGKAbilitySystemComponent* AbilitySystemComponent;
    class UGKAttributeSet*           AttributeSet;
    
    FGameplayTag TagDead;
    FGameplayTag TagDeathDispel;
  
public:
    UPROPERTY(BlueprintAssignable, Category = "Death")
    FCharacterDiedDelegate OnCharacterDied;
    
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Death")
    UAnimMontage* DeathMontage;
    
    UFUNCTION(BlueprintCallable, Category = "Death")
    void Die();
};
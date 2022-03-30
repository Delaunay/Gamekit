// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Gamekit.h"

// Unreal Engine
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"

// Generated
#include "GKAttributeSet.generated.h"

#if defined(DOXYGEN_DOCUMENTATION_BUILD)
#    define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)
#else
// Uses macros from AttributeSet.h
#    define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)           \
        GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
        GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)               \
        GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)               \
        GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
#endif

/** This holds all of the attributes used by abilities, it instantiates a copy of this on every character
 *
 * Do not update attributes directly, this should be handled by a Gameplay effect to not bypass the reflection
 */
UCLASS(Blueprintable)
class GAMEKIT_API UGKAttributeSet: public UAttributeSet
{
    GENERATED_BODY()
    public:
    // Constructor and overrides
    UGKAttributeSet();
    virtual void PreAttributeChange(const FGameplayAttribute &Attribute, float &NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData &Data) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

    /** Current Health, when 0 we expect owner to die. Capped by MaxHealth */
    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, Health)

    /** MaxHealth is its own attribute, since GameplayEffects may modify it */
    UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, MaxHealth)

    /** Current Mana, used to execute special abilities. Capped by MaxMana */
    UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, Mana)

    /** MaxMana is its own attribute, since GameplayEffects may modify it */
    UPROPERTY(BlueprintReadOnly, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, MaxMana)

    /** AttackPower of the attacker is multiplied by the base Damage to reduce health, so 1.0 means no bonus */
    UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_AttackPower)
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, AttackPower)

    /** Base Damage is divided by DefensePower to get actual damage done, so 1.0 means no bonus */
    UPROPERTY(BlueprintReadOnly, Category = "Damage", ReplicatedUsing = OnRep_DefensePower)
    FGameplayAttributeData DefensePower;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, DefensePower)

    /** MoveSpeed affects how fast characters can move */
    UPROPERTY(BlueprintReadOnly, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, MoveSpeed)

    /** Damage is a 'temporary' attribute used by the DamageExecution to calculate final damage, which then turns into
     * -Health */
    UPROPERTY(BlueprintReadOnly, Category = "Damage")
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, Damage)

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    FGameplayAttributeData GetAttribute(FName AttributeName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    float GetBaseValue(FName AttributeName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    float GetCurrentValue(FName AttributeName);

    protected:
    /** Helper function to proportionally adjust the value of an attribute when it's associated max attribute changes.
     * (i.e. When MaxHealth increases, Health increases by an amount that maintains the same percentage as before) */
    void AdjustAttributeForMaxChange(FGameplayAttributeData &      AffectedAttribute,
                                     const FGameplayAttributeData &MaxAttribute,
                                     float                         NewMaxValue,
                                     const FGameplayAttribute &    AffectedAttributeProperty);

    // These OnRep functions exist to make sure that the ability system internal representations are synchronized
    // properly during replication
    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData &OldValue);

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData &OldValue);

    UFUNCTION()
    virtual void OnRep_Mana(const FGameplayAttributeData &OldValue);

    UFUNCTION()
    virtual void OnRep_MaxMana(const FGameplayAttributeData &OldValue);

    UFUNCTION()
    virtual void OnRep_AttackPower(const FGameplayAttributeData &OldValue);

    UFUNCTION()
    virtual void OnRep_DefensePower(const FGameplayAttributeData &OldValue);

    UFUNCTION()
    virtual void OnRep_MoveSpeed(const FGameplayAttributeData &OldValue);

    TMap<FName, FGameplayAttributeData> NameToAttribute;
};

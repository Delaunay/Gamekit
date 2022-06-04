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
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Health", ReplicatedUsing = OnRep_Health)
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, Health)

    /** MaxHealth is its own attribute, since GameplayEffects may modify it */
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Health", ReplicatedUsing = OnRep_MaxHealth)
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, MaxHealth)

    /** Current Mana, used to execute special abilities. Capped by MaxMana */
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Mana", ReplicatedUsing = OnRep_Mana)
    FGameplayAttributeData Mana;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, Mana)

    /** MaxMana is its own attribute, since GameplayEffects may modify it */
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Mana", ReplicatedUsing = OnRep_MaxMana)
    FGameplayAttributeData MaxMana;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, MaxMana)

    /** MoveSpeed affects how fast characters can move */
    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MoveSpeed", ReplicatedUsing = OnRep_MoveSpeed)
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, MoveSpeed)

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Damage", ReplicatedUsing = OnRep_Damage)
    FGameplayAttributeData Damage;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, Damage)

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Armor", ReplicatedUsing = OnRep_Armor)
    FGameplayAttributeData Armor;
    ATTRIBUTE_ACCESSORS(UGKAttributeSet, Armor)

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    FGameplayAttributeData GetAttribute(FName AttributeName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    float GetBaseValue(FName AttributeName);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
    float GetCurrentValue(FName AttributeName);

    void SetMovementComponentMaxSpeed(float Value);

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
    virtual void OnRep_MaxMana(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    virtual void OnRep_Damage(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    virtual void OnRep_Armor(const FGameplayAttributeData& OldValue);

    TMap<FName, FGameplayAttributeData> NameToAttribute;
};

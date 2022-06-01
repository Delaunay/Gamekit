// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "GamekitEd/Abilities/GKGameplayAbilityFactory.h"


#include "Gamekit/Abilities/GKGameplayAbility.h"

#define LOCTEXT_NAMESPACE "GKGameplayAbilityFactory"


UGKGameplayAbilityFactory::UGKGameplayAbilityFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UGKGameplayAbility::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}


UObject* UGKGameplayAbilityFactory::FactoryCreateNew(UClass* Class,
                                                     UObject* InParent,
                                                     FName Name,
                                                     EObjectFlags Flags,
                                                     UObject* Context,
                                                     FFeedbackContext* Warn)
{
    UGKGameplayAbility* Ability = nullptr;

    if (ensure(SupportedClass == Class))
    {
        ensure(0 != (RF_Public & Flags));
        Ability = NewObject<UGKGameplayAbility>(InParent, Name, Flags);

        if (Ability)
        {

        }
    }

    return Ability;
}
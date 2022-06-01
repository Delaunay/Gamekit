// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.
#pragma once


// GamekitEd
#include "GamekitEd/GamekitEd.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"

// Generated
#include "GKGameplayAbilityFactory.generated.h"

UCLASS(hidecategories=Object, collapsecategories)
class GAMEKITED_API UGKGameplayAbilityFactory : public UFactory
{
    GENERATED_UCLASS_BODY()

    virtual UObject* FactoryCreateNew(
        UClass* Class,UObject* InParent,
        FName Name,
        EObjectFlags Flags,
        UObject* Context,
        FFeedbackContext* Warn) override;
};


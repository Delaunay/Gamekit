// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"

// Generated
#include "GKSelectableInterface.generated.h"

UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UGKSelectableInterface: public UInterface
{
    GENERATED_BODY()
};

class GAMEKIT_API IGKSelectableInterface
{
    public:
    GENERATED_BODY()

    UFUNCTION(BlueprintCallable, Category = "UnitSelection")
    virtual void Select() = 0;

    UFUNCTION(BlueprintCallable, Category = "UnitSelection")
    virtual void Deselect() = 0;
};

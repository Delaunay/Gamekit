// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GKSelectableInterface.generated.h"


UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class UGKSelectableInterface: public UInterface {
    GENERATED_BODY()
};

class GAMEKIT_API IGKSelectableInterface {
    public:

    GENERATED_BODY()

    void Select();

    void Deselect();
};

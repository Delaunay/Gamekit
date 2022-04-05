// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGamekit, Log, All);

#define GKGETATTR(Obj, Attr, Default) Obj ? Obj->Attr : Default

class FGamekitModule: public IModuleInterface
{
    public:
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;
};

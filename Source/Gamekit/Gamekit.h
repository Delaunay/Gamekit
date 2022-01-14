// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGamekit, Log, All);

class FGamekitModule: public IModuleInterface {
public:
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;
};

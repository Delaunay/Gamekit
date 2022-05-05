// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "Factories/Factory.h"

// Generated
#include "LuaScriptFactory.generated.h"

/**
 * 
 */
UCLASS()
class GAMEKIT_API ULuaScriptFactory : public UFactory
{
    GENERATED_BODY()

public:
    ULuaScriptFactory(const FObjectInitializer& ObjectInitializer);

    bool FactoryCanImport(const FString& Filename);

    // Create a new ULuaScript that just saves the path to the script
    // Other engine can now reference the script using the ULuaScript
    UObject* FactoryCreateFile(UClass* InClass,
                                UObject* InParent,
                                FName InName,
                                EObjectFlags Flags,
                                const FString& Filename,
                                const TCHAR* Parms,
                                FFeedbackContext* Warn,
                                bool& bOutOperationCanceled);

};

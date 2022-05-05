// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Lua/LuaScriptFactory.h"

// Gamekit
#include "Gamekit/Lua/LuaScript.h"


ULuaScriptFactory::ULuaScriptFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = ULuaScriptFactory::StaticClass();
    Formats.Add(TEXT("lua;Script lua"));

    bCreateNew = true;
    bText = false;
    bEditorImport = true;
    bEditAfterNew = false;
}


bool ULuaScriptFactory::FactoryCanImport(const FString& Filename)
{
    const FString Extension = FPaths::GetExtension(Filename);

    if (Extension == TEXT("lua"))
    {
        return true;
    }

    return false;
}

UObject* ULuaScriptFactory::FactoryCreateFile(UClass* InClass,
                                                UObject* InParent, 
                                                FName InName, 
                                                EObjectFlags Flags, 
                                                const FString& Filename,
                                                const TCHAR* Parms,
                                                FFeedbackContext* Warn, 
                                                bool& bOutOperationCanceled)
{
    ULuaScript* CustomData = NewObject<ULuaScript>(InParent, InClass, InName, Flags);
    CustomData->LuaScriptPath = Filename;

    return CustomData;
}
// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Lua/GKLuaScriptFactory.h"

// Gamekit
#include "Gamekit/Lua/GKLuaScript.h"


UGKLuaScriptFactory::UGKLuaScriptFactory(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    SupportedClass = UGKLuaScript::StaticClass();
    Formats.Add(TEXT("lua;Lua Script"));

    // Can we create a new object from scratch 
    bCreateNew = false;

    // Create from text (read from a file)
    bText = false;

    // Create from a filename
    bEditorImport = true;

    bEditAfterNew = false;

    // Low priority because this is a Experimental plugin
    ImportPriority = 0;
}


bool UGKLuaScriptFactory::FactoryCanImport(const FString& Filename)
{
    const FString Extension = FPaths::GetExtension(Filename);

    if (Extension == TEXT("lua"))
    {
        return true;
    }

    GKLUA_LOG(TEXT("Cannot import %s"), *Filename);
    return false;
}

UObject* UGKLuaScriptFactory::FactoryCreateFile(UClass* InClass,
                                                UObject* InParent, 
                                                FName InName, 
                                                EObjectFlags Flags, 
                                                const FString& Filename,
                                                const TCHAR* Parms,
                                                FFeedbackContext* Warn, 
                                                bool& bOutOperationCanceled)
{

    GKLUA_LOG(TEXT("Import new file %s"), *Filename);

    // The path here is relative ./../../../Chessy/Content/myscript.lua
    // we would need to workout the path using the content path
    // or plugin path

    // Example: ../../../../Chessy/
    static FString ProjectDir = FPaths::ProjectDir();

    // Example: ../../../../Chessy/Content/myscript.lua
    auto NewFilename = Filename;

    // New path is `Content/myscript.lua`
    //
    //  TODO: might have to replace Content by /Game
    //        and replace Plugins/Gamekit/Content by /Gamekit instead
    //        to handle better relocation of scripts
    if (NewFilename.RemoveFromStart(ProjectDir)) {
        UGKLuaScript* CustomData = NewObject<UGKLuaScript>(InParent, InClass, InName, Flags);
        CustomData->LuaScriptPath = NewFilename;

        bOutOperationCanceled = false;
        return CustomData;
    }

    GKLUA_LOG(TEXT("Could not work out the script path from the project's folder"));
    return nullptr;
}
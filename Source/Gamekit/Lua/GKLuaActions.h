// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Gamekit
#include "Gamekit/Lua/GKLua.h"

// Unreal Engine
#include "CoreMinimal.h"
#include "KismetCompilerModule.h"
#include "AssetTypeActions_Base.h"
#include "Misc/MessageDialog.h"

class FGKAssetTypeActions_LuaScript : public FAssetTypeActions_Base
{
public:
    virtual FText GetName() const override;

    virtual FColor GetTypeColor() const override;
    
    virtual UClass* GetSupportedClass() const override;
    
    virtual uint32 GetCategories() override;
    
    virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
    
    virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
    
    virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;
};

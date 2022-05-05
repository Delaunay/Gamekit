// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Lua/LuaActions.h"

// Gamekit
#include "Gamekit/Lua/LuaScript.h"

// IAssetTypeActions Implementation
FText FAssetTypeActions_LuaScript::GetName() const {
    return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_LuaScript", "LuaScript");
}

FColor FAssetTypeActions_LuaScript::GetTypeColor() const {
    // Blue from Lua's logo 
    return FColor(0, 0, 128);
}

UClass* FAssetTypeActions_LuaScript::GetSupportedClass() const { 
    return ULuaScript::StaticClass(); 
}

uint32 FAssetTypeActions_LuaScriptLLGetCategories() { 
    return EAssetTypeCategories::Gameplay; 
}

bool FAssetTypeActions_LuaScript::HasActions(const TArray<UObject*>& InObjects) const {
    return true; 
}

void FAssetTypeActions_LuaScript::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) {

}

void FAssetTypeActions_LuaScript::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) {

}
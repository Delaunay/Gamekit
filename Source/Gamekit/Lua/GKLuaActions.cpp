// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Lua/GKLuaActions.h"

// Gamekit
#include "Gamekit/Lua/GKLuaScript.h"

// IAssetTypeActions Implementation
FText FGKAssetTypeActions_LuaScript::GetName() const {
    return NSLOCTEXT("AssetTypeActions", "FAssetTypeActions_LuaScript", "LuaScript");
}

FColor FGKAssetTypeActions_LuaScript::GetTypeColor() const {
    // Blue from Lua's logo 
    return FColor(0, 0, 128);
}

UClass* FGKAssetTypeActions_LuaScript::GetSupportedClass() const {
    return UGKLuaScript::StaticClass(); 
}

uint32 FGKAssetTypeActions_LuaScript::GetCategories() {
    return EAssetTypeCategories::Gameplay; 
}

bool FGKAssetTypeActions_LuaScript::HasActions(const TArray<UObject*>& InObjects) const {
    return true; 
}

void FGKAssetTypeActions_LuaScript::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor) {

}

void FGKAssetTypeActions_LuaScript::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) {

}

// TODO: need to register action on module load
void Register() {
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
    AssetTools.RegisterAssetTypeActions(MakeShareable(new FGKAssetTypeActions_LuaScript));
}
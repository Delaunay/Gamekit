// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#include "GKAssetManager.h"

#include "Items/GKItem.h"

#include "AbilitySystemGlobals.h"

UGKAssetManager& UGKAssetManager::Get()
{
	UGKAssetManager* This = Cast<UGKAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogGamekit, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be GKAssetManager!"));
		return *NewObject<UGKAssetManager>(); // never calls this
	}
}

void UGKAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
}


UGKItem* UGKAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UGKItem* LoadedItem = Cast<UGKItem>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogGamekit, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}
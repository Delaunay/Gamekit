// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#pragma once

#include "Gamekit.h"
#include "Engine/AssetManager.h"
#include "GKAssetManager.generated.h"

class UGKItem;

/**
 * This was used to initialize the GAS when Subsystem did not exist yet
 * Currently it is useless
 */
UCLASS()
class GAMEKIT_API UGKAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UGKAssetManager() {}
	virtual void StartInitialLoading() override;

	/** Returns the current AssetManager object */
	static UGKAssetManager& Get();

	/**
	 * Synchronously loads an GKItem subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bDisplayWarning If true, this will log a warning if the item failed to load
	 */
	UGKItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};


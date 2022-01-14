// BSD 3-Clause License Copyright (c) 2021, Pierre Delaunay All rights reserved.

#include "GKSaveGame.h"
#include "GKGameInstance.h"

void UGKSaveGame::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	if (Ar.IsLoading() && SavedDataVersion != EGKSaveGameVersion::LatestVersion)
	{
		// Handle migration from old save to new save here
		SavedDataVersion = EGKSaveGameVersion::LatestVersion;
	}
}
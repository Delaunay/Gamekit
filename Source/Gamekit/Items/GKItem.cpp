// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Items/GKItem.h"

bool UGKItem::IsConsumable() const
{
    if (MaxCount <= 0)
    {
        return true;
    }
    return false;
}

FString UGKItem::GetIdentifierString() const { return GetPrimaryAssetId().ToString(); }

FPrimaryAssetId UGKItem::GetPrimaryAssetId() const
{
    // This is a DataAsset and not a blueprint so we can just use the raw FName
    // For blueprints you need to handle stripping the _C suffix
    return FPrimaryAssetId(ItemType, GetFName());
}
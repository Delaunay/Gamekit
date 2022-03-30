// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"

// Unreal Engine
#include "Camera/CameraComponent.h"
#include "Engine/CollisionProfile.h"

class UCollisionProfileHack
{
    public:
    TArray<ECollisionChannel> ObjectTypeMapping;
};

void UGKFogOfWarLibrary::ConvertToObjectType(ECollisionChannel                      CollisionChannel,
                                             TArray<TEnumAsByte<EObjectTypeQuery>> &ObjectTypes)
{
    if (CollisionChannel < ECC_MAX)
    {
        int32 ObjectTypeIndex = 0;
        // ofc it is private
        UCollisionProfileHack *CollisionProfile = reinterpret_cast<UCollisionProfileHack *>(UCollisionProfile::Get());

        for (const auto &MappedCollisionChannel: CollisionProfile->ObjectTypeMapping)
        {
            if (MappedCollisionChannel == CollisionChannel)
            {
                ObjectTypes.Add((EObjectTypeQuery)ObjectTypeIndex);
            }

            ObjectTypeIndex++;
        }
    }
}

void UGKFogOfWarLibrary::SetCameraPostprocessMaterial(AGKFogOfWarVolume *Volume,
                                                      FName              Faction,
                                                      UCameraComponent * CameraComponent)
{
    auto Material = Volume->GetFogOfWarPostprocessMaterial(Faction);

    if (Material == nullptr)
    {
        UE_LOG(LogGamekit, Log, TEXT("Postprocess Material was null, Are you using decal rendering?"));
        return;
    }

    for (auto &Blendable: CameraComponent->PostProcessSettings.WeightedBlendables.Array)
    {
        if (Blendable.Object == Material)
        {
            return;
        }
    }
    CameraComponent->PostProcessSettings.WeightedBlendables.Array.Add(FWeightedBlendable(1, Material));
}
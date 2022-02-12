// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"


#include "Engine/CollisionProfile.h"



class UCollisionProfileHack
{
    public:
	TArray<ECollisionChannel> ObjectTypeMapping;
};

void UGKFogOfWarLibrary::ConvertToObjectType(ECollisionChannel CollisionChannel, TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes)
{
	if (CollisionChannel < ECC_MAX)
	{
		int32 ObjectTypeIndex = 0;
		// ofc it is private
		UCollisionProfileHack* CollisionProfile = reinterpret_cast<UCollisionProfileHack*>(UCollisionProfile::Get());

		for(const auto& MappedCollisionChannel : CollisionProfile->ObjectTypeMapping)
		{
			if(MappedCollisionChannel == CollisionChannel)
			{
				ObjectTypes.Add((EObjectTypeQuery)ObjectTypeIndex);
			}

			ObjectTypeIndex++;
		}
	}
}
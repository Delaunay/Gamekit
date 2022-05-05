// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LuaScript.generated.h"

/**
 * Lua script asset, simply hold the path to the lua script
 */
UCLASS()
class GAMEKIT_API ULuaScript : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString LuaScriptPath;

	// Cached bytecote
	TArray<uint8> ByteCote;
};

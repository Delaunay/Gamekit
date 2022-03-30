#include "Gamekit/FogOfWar/Upscaler/GK_GPU_Upscaler.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/Shaders/GKUpscalingShader.h"


void UGKGPUUpscalerStrategy::Initialize()
{ 
    Super::Initialize();
    Multiplier   = 2;
    bFixedSize   = false;
    bInitialized = true;
    
    auto Grid           = FogOfWarVolume->Grid;
    auto MapSize        = FogOfWarVolume->MapSize;
    auto TileCountFloat = FVector(MapSize.X, MapSize.Y, 0) / Grid.GetTileSize();
    auto TileCount      = FIntVector(int(TileCountFloat.X), int(TileCountFloat.Y), int(TileCountFloat.Z));

    TextureSize = TileCount;

    UpscalingDispatcher = FUpscalingDispatcher::Get();
    UpscalingDispatcher->BeginRendering();
}

void UGKGPUUpscalerStrategy::Transform(struct FGKFactionFog *FactionFog)
{ 
    static uint32 CallCount = 0;

    auto Upscaled = GetFactionTransformTarget(FactionFog->Name, true);
    FactionFog->UpScaledVision = Upscaled;

    FUpscalingParameter Params;
    Params.OriginalTexture = FactionFog->Vision;
    Params.UpscaledTexture = Upscaled;
    Params.OriginalSize    = FIntPoint(TextureSize.X, TextureSize.Y);
    Params.TimeStamp       = ++CallCount;
    Params.Multiplier      = Multiplier;

    UpscalingDispatcher->UpdateParameters(Params);
}

void UGKGPUUpscalerStrategy::Stop() { 
    UpscalingDispatcher->EndRendering(); 
}
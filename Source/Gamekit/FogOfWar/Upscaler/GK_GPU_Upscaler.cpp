#include "Gamekit/FogOfWar/Upscaler/GK_GPU_Upscaler.h"

#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"

#include "Gamekit/Shaders/GKUpscalingShader.h"


void UGKGPUUpscalerStrategy::Initialize()
{ 
    Super::Initialize();
    bInitialized   = true;
    
    auto Grid           = FogOfWarVolume->Grid;
    auto MapSize        = FogOfWarVolume->MapSize;
    auto TileCountFloat = FVector(MapSize.X, MapSize.Y, 0) / Grid.GetTileSize();
    auto TileCount      = FIntVector(int(TileCountFloat.X), int(TileCountFloat.Y), int(TileCountFloat.Z));

    TextureSize = TileCount;

    UpscalingDispatcher = FUpscalingDispatcher::Get();
    UpscalingDispatcher->BeginRendering();
}

void UGKGPUUpscalerStrategy::Upscale(FName Name, TMatrix3D<uint8> const *Original, UTexture *Tex)
{ 
    static uint32 CallCount = 0;

    FUpscalingParameter Params;
    Params.OriginalTexture = Tex;
    Params.UpscaledTexture = GetFactionUpscaleTarget(Name, true);
    Params.OriginalSize    = FIntPoint(TextureSize.X, TextureSize.Y);
    Params.TimeStamp       = ++CallCount;
    Params.Multiplier      = Multiplier;

    UpscalingDispatcher->UpdateParameters(Params);
}

void UGKGPUUpscalerStrategy::Stop() { 
    UpscalingDispatcher->EndRendering(); 
}
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
    UpscaledBuffer.Init(0, TileCount.X * Multiplier, TileCount.Y * Multiplier, 1);

    UpscalingDispatcher = FUpscalingDispatcher::Get();
    UpscalingDispatcher->BeginRendering();
}

void UGKGPUUpscalerStrategy::Upscale(FName Name, TMatrix3D<uint8> const *Original, UTexture2D *Tex)
{ 
    static uint32 CallCount = 0;

    FUpscalingParameter Params;
    Params.OriginalTexture = Tex;
    Params.UpscaledTexture = GetFactionUpscaleTarget(Name);
    Params.OriginalSize    = FIntPoint(TextureSize.X, TextureSize.Y);
    Params.TimeStamp       = ++CallCount;

    UpscalingDispatcher->UpdateParameters(Params);
}

void UGKGPUUpscalerStrategy::Stop() { 
    UpscalingDispatcher->EndRendering(); 
}
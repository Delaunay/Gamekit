#include "Gamekit/FogOfWar/Upscaler/GK_Upscaler.h"

#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"


#include "Gamekit/FogOfWar/Strategy/GK_FoW_ShadowCasting.h"

#define TEXEL_2x2(a, b, c, d) uint8(uint8(a) | (uint8(b) << 1) | (uint8(c) << 2) | (uint8(d) << 3))


UGKUpscalerStrategy::UGKUpscalerStrategy() {

}


struct FTexel4x4
{
    uint8 pixels[4][4];
};

// clang-format off
TMap<Texel2x2, FTexel4x4> Texel2x2ToTexel4x4 = {
    {TEXEL_2x2(0, 0, 0, 0), {{	{0x00, 0x00, 0x00, 0x00},
                                {0x00, 0x00, 0x00, 0x00},
                                {0x00, 0x00, 0x00, 0x00},
                                {0x00, 0x00, 0x00, 0x00}	}}},
    {TEXEL_2x2(0, 0, 0, 1), {{	{0x00, 0x00, 0x00, 0x00}, 
                                {0x00, 0x00, 0x00, 0x00},
                                {0x00, 0x00, 0x00, 0x80}, 
                                {0x00, 0x00, 0x80, 0xFF}	}}},
    {TEXEL_2x2(0, 0, 1, 0), {{	{0x00, 0x00, 0x00, 0x00}, 
                                {0x00, 0x00, 0x00, 0x00},
                                {0x80, 0x00, 0x00, 0x00}, 
                                {0xFF, 0x80, 0x00, 0x00}	}}},
    {TEXEL_2x2(0, 0, 1, 1), {{	{0x00, 0x00, 0x00, 0x00}, 
                                {0x00, 0x00, 0x00, 0x00},
                                {0xFF, 0xFF, 0xFF, 0xFF}, 
                                {0xFF, 0xFF, 0xFF, 0xFF}	}}},
    {TEXEL_2x2(0, 1, 0, 0), {{	{0x00, 0x00, 0x80, 0xFF}, 
                                {0x00, 0x00, 0x00, 0x80},
                                {0x00, 0x00, 0x00, 0x00}, 
                                {0x00, 0x00, 0x00, 0x00}	}}},
    {TEXEL_2x2(0, 1, 0, 1), {{	{0x00, 0x00, 0xFF, 0xFF}, 
                                {0x00, 0x00, 0xFF, 0xFF},
                                {0x00, 0x00, 0xFF, 0xFF}, 
                                {0x00, 0x00, 0xFF, 0xFF}	}}},
    {TEXEL_2x2(0, 1, 1, 0), {{	{0x00, 0x00, 0x80, 0xFF}, 
                                {0x00, 0x00, 0x00, 0x80},
                                {0x80, 0x00, 0x00, 0x00}, 
                                {0xFF, 0x80, 0x00, 0x00}	}}},
    {TEXEL_2x2(0, 1, 1, 1), {{	{0x00, 0x80, 0xFF, 0xFF}, 
                                {0x80, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0xFF}, 
                                {0xFF, 0xFF, 0xFF, 0xFF}	}}},
    {TEXEL_2x2(1, 0, 0, 0), {{	{0xFF, 0x80, 0x00, 0x00}, 
                                {0x80, 0x00, 0x00, 0x00},
                                {0x00, 0x00, 0x00, 0x00}, 
                                {0x00, 0x00, 0x00, 0x00}	}}},
    {TEXEL_2x2(1, 0, 0, 1), {{	{0xFF, 0x80, 0x00, 0x00}, 
                                {0x80, 0x00, 0x00, 0x00},
                                {0x00, 0x00, 0x00, 0x80}, 
                                {0x00, 0x00, 0x80, 0xFF}	}}},
    {TEXEL_2x2(1, 0, 1, 0), {{	{0xFF, 0xFF, 0x00, 0x00}, 
                                {0xFF, 0xFF, 0x00, 0x00},
                                {0xFF, 0xFF, 0x00, 0x00}, 
                                {0xFF, 0xFF, 0x00, 0x00}	}}},
    {TEXEL_2x2(1, 0, 1, 1), {{	{0xFF, 0xFF, 0x80, 0x00}, 
                                {0xFF, 0xFF, 0xFF, 0x80},
                                {0xFF, 0xFF, 0xFF, 0xFF}, 
                                {0xFF, 0xFF, 0xFF, 0xFF}	}}},
    {TEXEL_2x2(1, 1, 0, 0), {{	{0xFF, 0xFF, 0xFF, 0xFF}, 
                                {0xFF, 0xFF, 0xFF, 0xFF},
                                {0x00, 0x00, 0x00, 0x00}, 
                                {0x00, 0x00, 0x00, 0x00}	}}},
    {TEXEL_2x2(1, 1, 0, 1), {{	{0xFF, 0xFF, 0xFF, 0xFF}, 
                                {0xFF, 0xFF, 0xFF, 0xFF},
                                {0x80, 0xFF, 0xFF, 0xFF}, 
                                {0x00, 0x80, 0xFF, 0xFF}	}}},
    {TEXEL_2x2(1, 1, 1, 0), {{	{0xFF, 0xFF, 0xFF, 0xFF}, 
                                {0xFF, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0x80}, 
                                {0xFF, 0xFF, 0x80, 0x00}	}}},
    {TEXEL_2x2(1, 1, 1, 1), {{	{0xFF, 0xFF, 0xFF, 0xFF}, 
                                {0xFF, 0xFF, 0xFF, 0xFF},
                                {0xFF, 0xFF, 0xFF, 0xFF}, 
                                {0xFF, 0xFF, 0xFF, 0xFF}	}}}
};           
// clang-format on


Texel2x2 UGKUpscalerStrategy::GetTexel(TMatrix3D<uint8> const &Mat, FIntVector v)
{
    int a = bool(Mat(v + FIntVector(0, 0, 0)) & (uint8)(EGK_TileVisbility::Visible));
    int b = bool(Mat(v + FIntVector(1, 0, 0)) & (uint8)(EGK_TileVisbility::Visible));
    int c = bool(Mat(v + FIntVector(0, 1, 0)) & (uint8)(EGK_TileVisbility::Visible));
    int d = bool(Mat(v + FIntVector(1, 1, 0)) & (uint8)(EGK_TileVisbility::Visible));
    return TEXEL_2x2(a, b, c, d);
}

void UGKUpscalerStrategy::Upscale()
{ 
    // this is too costly to do on the CPU
    // I am going to make it a GPU
}

void UGKUpscalerStrategy::Initialize() { 
    FogOfWarVolume = Cast<AGKFogOfWarVolume>(GetOwner());

    auto Grid           = FogOfWarVolume->Grid;
    auto MapSize        = FogOfWarVolume->MapSize;
    auto TileCountFloat = FVector(MapSize.X, MapSize.Y, 0) / Grid.GetTileSize();
    auto TileCount      = FIntVector(int(TileCountFloat.X), int(TileCountFloat.Y), int(TileCountFloat.Z));

    UpscaledBuffer.Init(0, TileCount.X * 2.f, TileCount.Y * 2.f, 1);
}


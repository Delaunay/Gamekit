#include "Gamekit/FogOfWar/Upscaler/GK_CPU_Upscaler.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/FogOfWar/Strategy/GK_FoW_ShadowCasting.h"
#include "Gamekit/FogOfWar/GKFogOfWarActorTeam.h"


#define TEXEL_2x2(a, b, c, d) uint8(uint8(a) << 3 | (uint8(b) << 2) | (uint8(c) << 1) | (uint8(d)))

#define IS_VISIBLE(X) bool(X & (uint8)(EGK_TileVisbility::Visible))

#define FULL 0xFF
#define HALF 0X80

// clang-format off
static uint8_t PatternMap[256] = {
    // TEXEL_2x2_0000
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    // TEXEL_2x2_0001
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, HALF,
    0x00, 0x00, HALF, FULL,
    // TEXEL_2x2_0010
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    HALF, 0x00, 0x00, 0x00,
    FULL, HALF, 0x00, 0x00,
    // TEXEL_2x2_0011
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL,
    // TEXEL_2x2_0100
    0x00, 0x00, HALF, FULL,
    0x00, 0x00, 0x00, HALF,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    // TEXEL_2x2_0101
    0x00, 0x00, FULL, FULL,
    0x00, 0x00, FULL, FULL,
    0x00, 0x00, FULL, FULL,
    0x00, 0x00, FULL, FULL,
    // TEXEL_2x2_0110
    0x00, 0x00, HALF, FULL,
    0x00, 0x00, 0x00, HALF,
    HALF, 0x00, 0x00, 0x00,
    FULL, HALF, 0x00, 0x00,
    // TEXEL_2x2_0111
    0x00, HALF, FULL, FULL,
    HALF, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL,
    // TEXEL_2x2_1000
    FULL, HALF, 0x00, 0x00,
    HALF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    // TEXEL_2x2_1001
    FULL, HALF, 0x00, 0x00,
    HALF, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, HALF,
    0x00, 0x00, HALF, FULL,
    // TEXEL_2x2_1010
    FULL, FULL, 0x00, 0x00,
    FULL, FULL, 0x00, 0x00,
    FULL, FULL, 0x00, 0x00,
    FULL, FULL, 0x00, 0x00,
    // TEXEL_2x2_1011
    FULL, FULL, HALF, 0x00,
    FULL, FULL, FULL, HALF,
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL,
    // TEXEL_2x2_1100
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    // TEXEL_2x2_1101
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL,
    HALF, FULL, FULL, FULL,
    0x00, HALF, FULL, FULL,
    // TEXEL_2x2_1110
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, HALF,
    FULL, FULL, HALF, 0x00,
    // TEXEL_2x2_1111
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL,
    FULL, FULL, FULL, FULL
};
// clang-format on

Texel2x2 UGKCPUUpscalerStrategy::GetTexel(TMatrix3D<uint8> const &Mat, FIntVector v)
{

    int a = IS_VISIBLE(Mat.GetWithDefault(v + FIntVector(0, 0, 0), 0));
    int b = IS_VISIBLE(Mat.GetWithDefault(v + FIntVector(1, 0, 0), 0));
    int c = IS_VISIBLE(Mat.GetWithDefault(v + FIntVector(0, 1, 0), 0));
    int d = IS_VISIBLE(Mat.GetWithDefault(v + FIntVector(1, 1, 0), 0));

    return TEXEL_2x2(a, b, c, d);
}

void UGKCPUUpscalerStrategy::Initialize()
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
    UpscaledBuffer.Init(0, TileCount.X * Multiplier, TileCount.Y * Multiplier, 1);
}

void UGKCPUUpscalerStrategy::Transform(class AGKFogOfWarActorTeam *FactionFog)
{
    if (FactionFog->Buffer == nullptr)
    {
        return;
    }

    auto Upscaled              = GetFactionTransformTarget(FactionFog->Name, true);
    FactionFog->UpScaledVision = Upscaled;
    auto Original              = static_cast<TMatrix3D<uint8> *>(FactionFog->Buffer);

    const uint8 PAT_WIDTH = 4;

    for (int32 y = 0; y < Original->Height(); ++y)
    {
        for (int32 x = 0; x < Original->Width(); ++x)
        {
            // Fetch original texture pixel and its neighbors
            Texel2x2 Texel = GetTexel(*Original, FIntVector(x, y, 0));

            uint8_t *UpscaledTexel = PatternMap + Texel * 16;

            for (int32 i = 0; i < PAT_WIDTH; ++i)
            {
                for (int32 j = 0; j < PAT_WIDTH; ++j)
                {
                    FIntVector Pos(x * Multiplier + j, y * Multiplier + i, 0);

                    if (UpscaledBuffer.Valid(Pos))
                        UpscaledBuffer(Pos) = UpscaledTexel[j + i * PAT_WIDTH];
                }
            }
        }
    }

    uint8 *NewBuffer = new uint8[UpscaledBuffer.GetLayerSize()];
    FMemory::Memcpy(NewBuffer, UpscaledBuffer.GetLayer(0), UpscaledBuffer.GetLayerSizeBytes());

    UpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, UpscaledBuffer.Width(), UpscaledBuffer.Height());
    Upscaled->UpdateTextureRegions(0,
                                   1,
                                   &UpdateRegion,
                                   UpdateRegion.Width,
                                   sizeof(uint8),
                                   NewBuffer,
                                   [&](uint8 *Buf, const FUpdateTextureRegion2D *) { delete[] Buf; });
}



#include "Gamekit/FogOfWar/Strategy/GK_FoW_ShadowCasting.h"

#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "Rendering/Texture2DResource.h"

#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"


UTexture *UGKShadowCasting::GetFactionTexture(FName Name, bool CreateRenderTarget)
{
    return GetFactionTexture2D(Name, CreateRenderTarget);
}


UTexture2D *UGKShadowCasting::CreateTexture2D() {
    auto Texture = UTexture2D::CreateTransient(Buffer.Width(), Buffer.Height(), EPixelFormat::PF_G8);

    Texture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
    Texture->SRGB                = false;
    Texture->Filter              = TextureFilter::TF_Nearest;
    Texture->AddressX            = TextureAddress::TA_Clamp;
    Texture->AddressY            = TextureAddress::TA_Clamp;
    Texture->MipGenSettings      = TextureMipGenSettings::TMGS_NoMipmaps;

    // Streaming Texture cause issues when updating it from the buffer
    // Texture->VirtualTextureStreaming = 1;
    // Texture->NeverStream = 0;

    Texture->UpdateResource();
    return Texture;
}

UTexture2D *UGKShadowCasting::GetFactionTexture2D(FName name, bool CreateRenderTarget)
{
    UTexture2D ** Result = FogFactions.Find(name);
    UTexture2D* Texture = nullptr;

    if (Result != nullptr)
    {
        Texture = Result[0];
    }
    else if (CreateRenderTarget)
    {   
        UE_LOG(LogGamekit, Log, TEXT("Creating a Texture for faction %s"), *name.ToString());
        Texture = CreateTexture2D();
        FogFactions.Add(name, Texture);
    }

    return Texture;
}

void UGKShadowCasting::UpdateBlocking(class UGKFogOfWarComponent *c)
{
    auto Actor = c->GetOwner();

    FVector Origin;
    FVector BoxExtent;
    Actor->GetActorBounds(true, Origin, BoxExtent);
    BoxExtent.Z = 0;

    FVector TopLeftCorner  = Origin - BoxExtent;
    FVector BotRightCorner = Origin + BoxExtent;

    /*
    float   Yaw            = Actor->GetActorRotation().Yaw;

    float Angle;
    float Rad;

    FMath::CartesianToPolar(TopLeftCorner.X, TopLeftCorner.Y, Rad, Angle);
    FMath::PolarToCartesian(Rad, Angle - FMath::DegreesToRadians(Yaw), TopLeftCorner.Z, TopLeftCorner.Z);

    FMath::CartesianToPolar(BotRightCorner.X, BotRightCorner.Y, Rad, Angle);
    FMath::PolarToCartesian(Rad, Angle + FMath::DegreesToRadians(Yaw), BotRightCorner.Z, BotRightCorner.Z);
    */

    auto TopLeftCornerGrid = Grid.WorldToGrid(TopLeftCorner);
    auto BotRightCornerGrid = Grid.WorldToGrid(BotRightCorner);

    UE_LOG(LogGamekit,
           Log,
           TEXT("Update Blocking From %s to %s"),
           *TopLeftCornerGrid.ToString(),
           *BotRightCornerGrid.ToString());

    Buffer.FillRectangle2D(FogOfWarVolume->ToGridTexture(TopLeftCornerGrid),
                           FogOfWarVolume->ToGridTexture(BotRightCornerGrid),
                           (uint8)(EGK_TileVisbility::Wall) 
    );
}

void UGKShadowCasting::UpdateTextures(class UTexture2D* Texture)
{
    UpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, Buffer.Width(), Buffer.Height());

    uint8 *NewBuffer = new uint8[Buffer.GetLayerSize()];
    FMemory::Memcpy(NewBuffer, Buffer.GetLayer(0), Buffer.GetLayerSizeBytes());

    Texture->UpdateTextureRegions(
        0, 
        1, 
        &UpdateRegion, 
        UpdateRegion.Width, 
        sizeof(uint8), 
        NewBuffer, 
        [](uint8* Buf, const FUpdateTextureRegion2D*) {
            delete[] Buf;
        }
    );
}

void UGKShadowCasting::DrawFactionFog()
{
    Buffer.ResetLayer(0, (uint8)(EGK_TileVisbility::None));
    TSet<FName> Factions;

    // Update the blocks
    for (auto &Component: FogOfWarVolume->ActorComponents)
    {
        if (Component->BlocksVision)
        {
            UpdateBlocking(Component);
        }
    }

    for (auto &Component: FogOfWarVolume->ActorComponents)
    {
        Factions.Add(Component->Faction);
        if (Component->GivesVision)
        {
            DrawLineOfSight(Component);
            
        }
    }

    // Update Textures for rendering
    for (auto Faction: Factions)
    {
        UpdateTextures(GetFactionTexture2D(Faction));
    }
}

void UGKShadowCasting::Initialize()
{
    Super::Initialize();

    Grid                = FogOfWarVolume->Grid;
    auto MapSize        = FogOfWarVolume->MapSize;
    auto TileCountFloat = FVector(MapSize.X, MapSize.Y, 0) / Grid.GetTileSize();
    auto TileCount      = FIntVector(int(TileCountFloat.X), int(TileCountFloat.Y), int(TileCountFloat.Z));

    Buffer.Init(0, TileCount.X, TileCount.Y, 1);
    FogOfWarVolume->SetTextureSize(FVector2D(TileCount.X, TileCount.Y));

    //*
    UE_LOG(LogGamekit, Log, TEXT("Map Size is %s"), *MapSize.ToString());
    UE_LOG(LogGamekit, Log, TEXT("TileCount Count is %s"), *TileCount.ToString());
    UE_LOG(LogGamekit, Log, TEXT("TileSize is %s"), *Grid.GetTileSize().ToString());
    //*/

    // Try to allocate our buffer early
    for (auto &Component: FogOfWarVolume->ActorComponents)
    {
        GetFactionTexture(Component->Faction);
    }
}

void UGKShadowCasting::DrawLineOfSight(UGKFogOfWarComponent *c)
{
    auto WorldPos = c->GetOwner()->GetActorLocation();

    auto GridCoord = Grid.WorldToGrid(WorldPos);
    auto Radius    = int(c->Radius / Grid.GetTileSize().X);

    /*
    UE_LOG(LogGamekit, Log, TEXT("Actor is %s"), *AActor::GetDebugName(c->GetOwner()));
    UE_LOG(LogGamekit, Log, TEXT("Grid Position is %s -> %s "), *GridCoord.ToString(), *WorldPos.ToString());
    UE_LOG(LogGamekit, Log, TEXT("Radius is %f %d"), c->Radius, Radius);
    //*/

    Compute(GridCoord, Radius);
}

bool UGKShadowCasting::BlocksLight(int X, int Y)
{
    auto TexturePos = FogOfWarVolume->ToGridTexture(FIntVector(X, Y, 0));

    if (Buffer.Valid(TexturePos))
    {
        // Check if wall flag is set
        auto Blocks = Buffer(TexturePos) & (uint8)(EGK_TileVisbility::Wall);
        if (Blocks)
        {
            UE_LOG(LogGamekit, Log, TEXT("Blocked %s!"), *TexturePos.ToString());
        }
        return bool(Blocks);
    }

    
    return true;
}

int UGKShadowCasting::GetDistance(FIntVector Origin, FIntVector Diff) { 
    Origin.Z = 0;
    Diff.Z   = 0;
    return (Origin - Diff).Size();
}

void UGKShadowCasting::SetVisible(int X, int Y)
{
    auto TexturePos = FogOfWarVolume->ToGridTexture(FIntVector(X, Y, 0));

    if (Buffer.Valid(TexturePos))
    {
        // Set visible flag
        Buffer(TexturePos) |= (uint8)(EGK_TileVisbility::Visible);
    }
    else
    {
        UE_LOG(LogGamekit, Log, TEXT("Cannot set %d x %d as visisble %s"), X, Y, *TexturePos.ToString());
    }
}

void UGKShadowCasting::Compute(FIntVector origin, int rangeLimit)
{
    SetVisible(origin.X, origin.Y);

    for (uint8 octant = 0; octant < 8; octant++) 
    {
        Compute(octant, origin, rangeLimit, 1, FGKSlope(1, 1), FGKSlope(0, 1));
    }
}

void UGKShadowCasting::Compute(uint8 octant, FIntVector origin, int rangeLimit, int x, FGKSlope top, FGKSlope bottom)
{
    // rangeLimit < 0 || x <= rangeLimit
    for (; (uint8)x <= (uint8)rangeLimit; x++)
    {
        // compute the Y coordinates where the top vector leaves the column (on the right) and where the bottom vector
        // enters the column (on the left). this equals (x+0.5)*top+0.5 and (x-0.5)*bottom+0.5 respectively, which can
        // be computed like (x+0.5)*top+0.5 = (2(x+0.5)*top+1)/2 = ((2x+1)*top+1)/2 to avoid floating point math
        int topY    = top.X == 1 ? x
                                 : ((x * 2 + 1) * top.Y + top.X - 1) / (top.X * 2); // the rounding is a bit tricky, though
        int bottomY = bottom.Y == 0 ? 0 : ((x * 2 - 1) * bottom.Y + bottom.X) / (bottom.X * 2);

        int wasOpaque = -1; // 0:false, 1:true, -1:not applicable
        for (int y = topY; y >= bottomY; y--)
        {
            int tx = origin.X, ty = origin.Y;

            // clang-format off
            switch (octant) // translate local coordinates to map coordinates
            {
                case 0: tx += x; ty -= y; break;
                case 1: tx += y; ty -= x; break;
                case 2: tx -= y; ty -= x; break;
                case 3: tx -= x; ty -= y; break;
                case 4: tx -= x; ty += y; break;
                case 5: tx -= y; ty += x; break;
                case 6: tx += y; ty += x; break;
                case 7: tx += x; ty += y; break;
            }
            // clang-format off

            bool inRange = rangeLimit < 0 || GetDistance(origin, FIntVector(tx, ty, 0)) <= rangeLimit;

            if (inRange){
                SetVisible(tx, ty);
            }

            // NOTE: use the next line instead if you want the algorithm to be symmetrical
            // if(inRange && (y != topY || top.Y*x >= top.X*y) && (y != bottomY || bottom.Y*x <= bottom.X*y)) SetVisible(tx, ty);

            bool isOpaque = !inRange || BlocksLight(tx, ty);
            if(x != rangeLimit)
            {
                if(isOpaque)
                {
                    // if we found a transition from clear to opaque, this sector is done in this column, so
                    // adjust the bottom vector upwards and continue processing it in the next column.
                    // (x*2-1, y*2+1) is a vector to the top-left of the opaque tile
                    if(wasOpaque == 0)
                    {
                        FGKSlope newBottom = FGKSlope(y*2+1, x*2-1);

                        if(!inRange || y == bottomY) {
                            // don't recurse unless we have to
                            bottom = newBottom;
                            break;
                        }
                        else {
                            Compute(octant, origin, rangeLimit, x+1, top, newBottom);
                        }
                    }
                    wasOpaque = 1;
                }
                else // adjust top vector downwards and continue if we found a transition from opaque to clear
                {    // (x*2+1, y*2+1) is the top-right corner of the clear tile (i.e. the bottom-right of the opaque tile)
                    if(wasOpaque > 0) {
                        top = FGKSlope(y*2+1, x*2+1);
                    }
                    wasOpaque = 0;
                }
            }
        }

        if(wasOpaque != 0) {
            // if the column ended in a clear tile, continue processing the current sector
            break;
        }
    }
}

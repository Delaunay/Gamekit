

#include "Gamekit/FogOfWar/Strategy/GK_FoW_ShadowCasting.h"

// Gamekit
#include "Gamekit/Blueprint/GKUtilityLibrary.h"
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/GKLog.h"

// Unreal Engine
#include "DrawDebugHelpers.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "Rendering/Texture2DResource.h"

void UGKShadowCasting::Stop() {}

UTexture *UGKShadowCasting::GetFactionTexture(FName Name, bool bCreateRenderTarget)
{
    return GetFactionTexture2D(Name);

    if (bPreviousIsPrevious)
    {
        return GetFactionTexture2D(Name);
    }
    else
    {
        return GetPreviousFrameFactionTexture2D(Name);
    }
}

UTexture *UGKShadowCasting::GetPreviousFrameFactionTexture(FName Name, bool bCreateRenderTarget)
{
    return GetPreviousFrameFactionTexture2D(Name);

    if (!bPreviousIsPrevious)
    {
        return GetFactionTexture2D(Name);
    }
    else
    {
        return GetPreviousFrameFactionTexture2D(Name);
    }
}

UTexture2D *UGKShadowCasting::CreateTexture2D()
{
    auto Texture = UTexture2D::CreateTransient(Buffer.Width(), Buffer.Height(), EPixelFormat::PF_G8);

    Texture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
    Texture->SRGB                = false;
    Texture->Filter              = TextureFilter::TF_Trilinear;
    Texture->AddressX            = TextureAddress::TA_Clamp;
    Texture->AddressY            = TextureAddress::TA_Clamp;
    Texture->MipGenSettings      = TextureMipGenSettings::TMGS_NoMipmaps;

    // Streaming Texture cause issues when updating it from the buffer
    // Texture->VirtualTextureStreaming = 1;
    // Texture->NeverStream = 0;
    Texture->UpdateResource();

    // UGKUtilityLibrary::ClearTexture(Texture, FLinearColor::Black);
    return Texture;
}

UTexture2D *UGKShadowCasting::GetFactionTexture2D(FName name, bool bCreateRenderTarget)
{
    UTexture2D **Result  = FogFactions.Find(name);
    UTexture2D * Texture = nullptr;

    if (Result != nullptr)
    {
        Texture = Result[0];
    }
    else if (bCreateRenderTarget && !IsBeingDestroyed())
    {
        UE_LOG(LogGamekit, Log, TEXT("Creating a Texture for faction %s"), *name.ToString());
        Texture = CreateTexture2D();
        FogFactions.Add(name, Texture);
    }

    return Texture;
}

bool UGKShadowCasting::IsVisible(FGenericTeamId SeerTeam, AActor const* Target) const {
    // this returns true only if the center of the actor is inside vision
    // THINK: should we check for its bounds ?
    if (SeerTeam == FGenericTeamId::NoTeam){
        return true;
    }

    auto ActorPosition = Grid.WorldToGrid(Target->GetActorLocation());

    auto BufferPos = FogOfWarVolume->ToGridTexture(FIntVector(ActorPosition.X, ActorPosition.Y, 0));

    BufferPos.Z = uint8(EGK_VisbilityLayers::Size) + SeerTeam.GetId();

    if (Buffer.Valid(BufferPos)) {
        return Buffer(BufferPos) & (uint8)(EGK_TileVisbility::Visible);
    }
    return false;
}

UTexture2D *UGKShadowCasting::GetPreviousFrameFactionTexture2D(FName name, bool bCreateRenderTarget)
{
    UTexture2D **Result  = PreviousFogFactions.Find(name);
    UTexture2D * Texture = nullptr;

    if (Result != nullptr)
    {
        Texture = Result[0];
    }

    else if (bCreateRenderTarget && !IsBeingDestroyed())
    {
        UE_LOG(LogGamekit, Log, TEXT("Creating a Texture for faction %s"), *name.ToString());
        Texture = CreateTexture2D();
        PreviousFogFactions.Add(name, Texture);
    }

    return Texture;
}

void UGKShadowCasting::UpdateBlocking(class UGKFogOfWarComponent *c)
{
    auto Actor = c->GetOwner();

    FVector Origin;
    FVector BoxExtent;
    Actor->GetActorBounds(true, Origin, BoxExtent);

    Origin.Z = 0;

    FTransform Transform;
    Transform.SetLocation(Origin);
    Transform.SetRotation(Actor->GetActorRotation().Quaternion());

    auto TopLeft  = Transform.TransformVector(Origin + BoxExtent * FVector(+1, -1, 0) * 0.90);
    auto TopRight = Transform.TransformVector(Origin + BoxExtent * FVector(+1, +1, 0) * 0.90);
    auto BotLeft  = Transform.TransformVector(Origin + BoxExtent * FVector(-1, -1, 0) * 0.90);
    auto BotRight = Transform.TransformVector(Origin + BoxExtent * FVector(-1, +1, 0) * 0.90);

    FVector Corners[4] = {TopLeft, TopRight, BotLeft, BotRight};

// Draw Corners
#if ENABLE_DRAW_DEBUG
    if (FogOfWarVolume->bDebug)
    {
        for (auto &Corner: Corners)
        {
            DebugDrawPoint(Corner, FLinearColor::White, 25.f);

            auto CornerGrid = Grid.SnapToGrid(Corner);

            DebugDrawPoint(CornerGrid, FLinearColor::Red, 30.f);
        }
    }
#endif

    //*
    auto TopLeftCornerGrid = Grid.WorldToGrid(TopRight);
    auto TopLeftTexure     = FogOfWarVolume->ToGridTexture(TopLeftCornerGrid);

    auto BotRightCornerGrid = Grid.WorldToGrid(BotLeft);
    auto BotRightTexture    = FogOfWarVolume->ToGridTexture(BotRightCornerGrid);

    int Width  = BotRightTexture.X - TopLeftTexure.X + 1;
    int Height = TopLeftTexure.Y - BotRightTexture.Y + 1;

    uint8 ZHeight = 0;
    // ZHeight = Origin.Z / TileSize.Z;
    uint8 Value = (uint8)(EGK_TileVisbility::Wall) | (ZHeight << 1);

    for (int j = 0; j < Height; j++)
    {
        for (int i = 0; i < Width; i++)
        {
            auto Pos    = TopLeftTexure + FIntVector(i, -j, uint8(EGK_VisbilityLayers::Blocking));
            Buffer(Pos) = Value;
        }
    }
}

void UGKShadowCasting::UpdateTextures(class AGKTeamFog* TeamFog)
{
#if !UE_SERVER
    if (GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    UpdatePreviousFrameTexturesTex(TeamFog);

    UTexture2D *Texture = GetFactionTexture2D(TeamFog->Name);
    bPreviousIsPrevious = !bPreviousIsPrevious;

    UpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, Buffer.Width(), Buffer.Height());

    uint8 *NewBuffer = new uint8[Buffer.GetLayerSize()];
    uint8 *SrcData = Buffer.GetLayer(uint8(EGK_VisbilityLayers::Size) + TeamFog->TeamId.GetId());

    FMemory::Memcpy(NewBuffer, SrcData, Buffer.GetLayerSizeBytes());

    Texture->UpdateTextureRegions(0,
                                  1,
                                  &UpdateRegion,
                                  UpdateRegion.Width,
                                  sizeof(uint8),
                                  NewBuffer,
                                  [&](uint8 *Buf, const FUpdateTextureRegion2D *) { delete[] Buf; });
#endif
}

void UGKShadowCasting::UpdatePreviousFrameTexturesTex(class AGKTeamFog* TeamFog)
{

    UTexture2D *Texture     = GetFactionTexture2D(TeamFog->Name);
    UTexture2D *PrevTexture = GetPreviousFrameFactionTexture2D(TeamFog->Name);
    /*
    ENQUEUE_RENDER_COMMAND(CopyTexture)
    (
            [Texture, PrevTexture](FRHICommandListImmediate &RHICmdList)
            {
                //FResolveParams Params;
                //RHICmdList.CopyToResolveTarget(
                //    Texture->GetResource()->GetTexture2DRHI(),
                //    PrevTexture->GetResource()->GetTexture2DRHI(),
                //    Params
                //);
                
                FRHICopyTextureInfo CopyParams;
                RHICmdList.CopyTexture(Texture->GetResource()->GetTexture2DRHI(),
                                       PrevTexture->GetResource()->GetTexture2DRHI(),
                                       CopyParams);
            });
    */
}

void UGKShadowCasting::UpdatePreviousFrameTextures(class AGKTeamFog* TeamFog)
{
#if !UE_SERVER
    if (Buffer.Num() > 0 && GetWorld()->GetNetMode() != NM_DedicatedServer)
    {
        // Copy Previous Frame Before reseting
        UTexture2D *PrevTexture = GetPreviousFrameFactionTexture2D(TeamFog->Name);

        UpdateRegion = FUpdateTextureRegion2D(0, 0, 0, 0, Buffer.Width(), Buffer.Height());
        
        uint8 *NewBuffer = new uint8[Buffer.GetLayerSize()];
        uint8* SrcData = Buffer.GetLayer(uint8(EGK_VisbilityLayers::Size) + TeamFog->TeamId.GetId());

        FMemory::Memcpy(NewBuffer, SrcData, Buffer.GetLayerSizeBytes());

        PrevTexture->UpdateTextureRegions(0,
                                          1,
                                          &UpdateRegion,
                                          UpdateRegion.Width,
                                          sizeof(uint8),
                                          NewBuffer,
                                          [&](uint8 *Buf, const FUpdateTextureRegion2D *) { delete[] Buf; });
    }
#endif
}

void UGKShadowCasting::DrawFactionFog(class AGKTeamFog *FactionFog)
{
    // Reset
    Buffer.ResetLayer(uint8(EGK_VisbilityLayers::Size) + FactionFog->TeamId.GetId(), (uint8)(EGK_TileVisbility::None));
    Buffer.ResetLayer(uint8(EGK_VisbilityLayers::Blocking), (uint8)(EGK_TileVisbility::None));

    CurrentFaction = FactionFog;

    FactionFog->Vision              = GetFactionTexture(FactionFog->Name, true);
    FactionFog->PreviousFrameVision = GetPreviousFrameFactionTexture(FactionFog->Name, true);
    FactionFog->Buffer              = static_cast<void *>(&Buffer);
    FactionFog->bDiscrete           = true;

    // FactionFog->GetBlocking(
    for (auto &Component: FogOfWarVolume->GetBlocking())
    {
        if (Component->BlocksVision)
        {
            UpdateBlocking(Component);
        }
    }

    for (auto &Component: FactionFog->Allies)
    {
        CurrentComponent = Component;
        if (Component->GivesVision)
        {
            DrawLineOfSight(FactionFog, Component);
            DebugDrawComponent(Component);
        }
    }

    // Update Textures for rendering
    UpdateTextures(FactionFog);
}

void UGKShadowCasting::Initialize()
{
    Super::Initialize();

    Grid                = FogOfWarVolume->Grid;
    auto MapSize        = FogOfWarVolume->MapSize;
    auto TileCountFloat = FVector(MapSize.X, MapSize.Y, 0) / Grid.GetTileSize();
    auto TileCount      = FIntVector(int(TileCountFloat.X), int(TileCountFloat.Y), int(TileCountFloat.Z));

    auto Settings = Cast<AGKWorldSettings>(GetWorld()->GetWorldSettings());
    auto TeamCount = GKGETATTR(Settings, GetTeams().Num(), 1);

    TextureSize = TileCount;
    Buffer.Init(0, TileCount.X, TileCount.Y, uint8(EGK_VisbilityLayers::Size) + TeamCount);
    FogOfWarVolume->SetTextureSize(FVector2D(TileCount.X, TileCount.Y));

    GK_LOG(TEXT("Map Size is %s"), *MapSize.ToString());
    GK_LOG(TEXT("TileCount Count is %s"), *TileCount.ToString());
    GK_LOG(TEXT("TileSize is %s"), *Grid.GetTileSize().ToString());
}

void UGKShadowCasting::DrawLineOfSight(class AGKTeamFog *FactionFog, UGKFogOfWarComponent *c)
{
    auto WorldPos = c->GetOwner()->GetActorLocation();

    auto       GridCoord       = Grid.WorldToGrid(WorldPos);
    auto       Radius          = int(c->Radius / Grid.GetTileSize().X);

    Compute(GridCoord, Radius, FactionFog->TeamId.GetId());
}

bool UGKShadowCasting::BlocksLight(int X, int Y)
{
    auto TexturePos = FogOfWarVolume->ToGridTexture(FIntVector(X, Y, 0));
    TexturePos.Z    = uint8(EGK_VisbilityLayers::Blocking);

    if (Buffer.Valid(TexturePos))
    {
        // TODO: Broadcast Sighting events?
        // Check if wall flag is set
        return bool(Buffer(TexturePos) & (uint8)(EGK_TileVisbility::Wall));
    }

    return true;
}

int UGKShadowCasting::GetDistance(FIntVector Origin, FIntVector Diff)
{
    Origin.Z = 0;
    Diff.Z   = 0;
    return (Origin - Diff).Size();
}

void UGKShadowCasting::SetVisible(int X, int Y, uint8 TeamId)
{
    auto BufferPos = FogOfWarVolume->ToGridTexture(FIntVector(X, Y, 0));
    BufferPos.Z    = uint8(EGK_VisbilityLayers::Size) + TeamId;

    if (Buffer.Valid(BufferPos))
    {
        // Set visible flag
        Buffer(BufferPos) |= (uint8)(EGK_TileVisbility::Visible);


        /* This cannot work because we would need to insert N units into the `PositionToComponent`
         * which would be very slow as N could be quite big 
        TexturePos.Z = 0;

        UGKFogOfWarComponent **SightedResult = PositionToComponent.Find(TexturePos);
        if (SightedResult != nullptr)
        {
            UGKFogOfWarComponent *Sighted = SightedResult[0];
            AddVisibleComponent(CurrentFaction, CurrentComponent, Sighted);
        }
        */
    }
    else
    {
        GK_LOG(TEXT("Cannot set %d x %d as visisble %s"), X, Y, *BufferPos.ToString());
    }
}

void UGKShadowCasting::Compute(FIntVector origin, int rangeLimit, uint8 TeamId)
{
    SetVisible(origin.X, origin.Y, TeamId);

    for (uint8 octant = 0; octant < 8; octant++)
    {
        Compute(octant, origin, rangeLimit, 1, FGKSlope(1, 1), FGKSlope(0, 1), TeamId);
    }
}

void UGKShadowCasting::Compute(uint8      octant,
                               FIntVector origin,
                               int        rangeLimit,
                               int        x,
                               FGKSlope   top,
                               FGKSlope   bottom,
                               uint8 TeamId)
{
    // rangeLimit < 0 || x <= rangeLimit
    for (; (uint8)x <= (uint8)rangeLimit; x++)
    {
        // compute the Y coordinates where the top vector leaves the column (on the right) and where the bottom vector
        // enters the column (on the left). this equals (x+0.5)*top+0.5 and (x-0.5)*bottom+0.5 respectively, which can
        // be computed like (x+0.5)*top+0.5 = (2(x+0.5)*top+1)/2 = ((2x+1)*top+1)/2 to avoid floating point math

        int topY      = top.X == 1 ? x
                                   : ((x * 2 + 1) * top.Y + top.X - 1) / (top.X * 2); // the rounding is a bit tricky, though
        int bottomY   = bottom.Y == 0 ? 0 : ((x * 2 - 1) * bottom.Y + bottom.X) / (bottom.X * 2);
        int wasOpaque = -1; // 0:false,
                            // 1:true, -1:not applicable

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
                SetVisible(tx, ty, TeamId);
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
                            Compute(octant, origin, rangeLimit, x+1, top, newBottom, TeamId);
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

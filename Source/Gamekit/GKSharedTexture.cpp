// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "GKSharedTexture.h"

#include "CanvasTypes.h"
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Textures/TextureAtlas.h"

// Sets default values for this component's properties
UGKSharedTexture::UGKSharedTexture() {}

// Called when the game starts
void UGKSharedTexture::BeginPlay()
{
    Super::BeginPlay();

    if (RenderTarget == nullptr)
        return;

    RenderTarget->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
    // RenderTarget->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    RenderTarget->SRGB = false;
    RenderTarget->UpdateResource();

    // Allocate a new texture for CPU queries
    TArray<uint8> AlphaOverride;
    CPUTexture = RenderTarget->ConstructTexture2D(this,         // UObject * Outer,
                                                  "CPUTexture", // const FString & NewTexName,
                                                  RF_NoFlags    // EObjectFlags InObjectFlags,
                                                                // 0,				// uint32 Flags,
                                                                // &AlphaOverride	// TArray<uint8>*AlphaOverride
    );

    UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTarget, FLinearColor(0, 0, 0, 0));
}

void UGKSharedTexture::AddPointCentered(FIntPoint Index, FLinearColor Value)
{
    AddPoint(FIntPoint(RenderTarget->SizeX / 2 - Index.X, RenderTarget->SizeY / 2 - Index.Y), Value);
}

FColor UGKSharedTexture::ReadCenteredPoint(FIntPoint Index)
{
    return ReadPoint(FIntPoint(RenderTarget->SizeX / 2 - Index.X, RenderTarget->SizeY / 2 - Index.Y));
}

void UGKSharedTexture::AddPoint(FIntPoint Index, FLinearColor Value)
{
    if (RenderTarget == nullptr)
        return;

    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, Canvas, Size, Context);

    FCanvasBoxItem BoxItem(Index, FIntPoint(1, 1));
    BoxItem.LineThickness = 1;
    BoxItem.SetColor(Value);
    Canvas->DrawItem(BoxItem);
}

ETextureSourceFormat GetTextureFormat(UCanvasRenderTarget2D *RenderTarget)
{
    const EPixelFormat PixelFormat = RenderTarget->GetFormat();
    switch (PixelFormat)
    {
    case PF_B8G8R8A8:
        return TSF_BGRA8;
    case PF_FloatRGBA:
        return TSF_RGBA16F;
    case PF_G8:
        return TSF_G8;
    }
    return TSF_Invalid;
}

FColor UGKSharedTexture::ReadPoint(FIntPoint p)
{
    RenderTarget->UpdateTexture2D(CPUTexture, GetTextureFormat(RenderTarget));
    const FColor *FormatedImageData = (const FColor *)(CPUTexture->PlatformData->Mips[0].BulkData.LockReadOnly());

    FColor PixelColor = FormatedImageData[p.Y * CPUTexture->GetSizeX() + p.X];
    CPUTexture->PlatformData->Mips[0].BulkData.Unlock();

    return PixelColor;
}

void UGKSharedTexture::RemovePoint(FIntPoint Index) { AddPoint(Index, FLinearColor(0, 0, 0, 0)); }

// Called every frame
// void USharedTexture::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction*
// ThisTickFunction)
// {
// 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
// 	// ...
// }

// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Ad/GKAdLibrary.h"

#include "ImageUtils.h"

FGKBidRequest UGKAdLibrary::MakeBidRequestMinimal() { return FGKBidRequest(); }

FGKBidRequest UGKAdLibrary::MakeBidRequest()
{
    // Try to set as many fields as possible here
    auto bidRequest = MakeBidRequestMinimal();

    return bidRequest;
}

UTexture2D *UGKAdLibrary::TextureFromURI(FString URI)
{
    FString URISpec;
    FString Base64Data;
    URI.Split(TEXT(","), &URISpec, &Base64Data);

    TArray<uint8> Bytes;
    bool          isDecoded = FBase64::Decode(Base64Data, Bytes);

    if (!isDecoded)
    {
        return nullptr;
    }

    return TextureFromPixels(Bytes);
}

UTexture2D *UGKAdLibrary::TextureFromPixels(TArray<uint8> &Buffer)
{
    UTexture2D *Texture = FImageUtils::ImportBufferAsTexture2D(Buffer);

    // Texture->MipGenSettings      = TMGS_NoMipmaps;
    Texture->CompressionSettings = TextureCompressionSettings::TC_Default;
    // Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
    Texture->SRGB   = false;
    Texture->Filter = TextureFilter::TF_Nearest;
    Texture->UpdateResource();
    return Texture;
}

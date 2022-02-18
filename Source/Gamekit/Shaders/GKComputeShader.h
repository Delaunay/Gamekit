#pragma once

#include "CoreMinimal.h"
#include "Math/IntVector.h"


#define CUSTOM_SHADER 1

#if CUSTOM_SHADER
struct FUspcalingShaderParameters
{
    class UTexture2D *            OriginalTexture;
    class UTextureRenderTarget2D *UpscaledTexture;
    FIntVector                    TextureSize;

    FIntPoint GetUpscaleSize() const { 
        auto UpscaleSize = TextureSize * 4;
        return FIntPoint(UpscaleSize.X, UpscaleSize.Y);
    }
};

// TODO: does this need to be a singleton ?
// I think not, the entity using should create a new one
// and manage it
class GAMEKIT_API FUpscalerShader
{
public:
    void BeginRendering();

    void EndRendering();

    void UpdateParameters(FUspcalingShaderParameters &DrawParameters);

    static FUpscalerShader *Get();

    private:

    FUpscalerShader();

    FUspcalingShaderParameters        Parameters;
    volatile bool                     bParamsValid;
    TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;
    TRefCountPtr<IPooledRenderTarget> ComputeShaderInput;
    FDelegateHandle                   OnPostResolvedSceneColorHandle;

    static FUpscalerShader *instance;

public:
    void Execute_RenderThread(FRHICommandListImmediate &RHICmdList, class FSceneRenderTargets &SceneContext);
};

#endif

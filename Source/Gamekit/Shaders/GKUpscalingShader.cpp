#include "Gamekit/Shaders/GKUpscalingShader.h"

#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"

#include "Modules/ModuleManager.h" 

#define NUM_THREADS_PER_GROUP_DIMENSION 32

class FUpscalingShader : public FGlobalShader
{
public:
    DECLARE_GLOBAL_SHADER(FUpscalingShader);
    SHADER_USE_PARAMETER_STRUCT(FUpscalingShader, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_TEXTURE(Texture2D<uint>, InputTexture)
        SHADER_PARAMETER_UAV(RWTexture2D<uint>, OutputTexture)
        SHADER_PARAMETER(FIntPoint, Dimensions)
        SHADER_PARAMETER(UINT, TimeStamp)
        SHADER_PARAMETER(UINT, Multiplier)
    END_SHADER_PARAMETER_STRUCT()

public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }

    static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
    {
        FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

        //We're using it here to add some preprocessor defines. That way we don't have to change both C++ and HLSL code when we change the value for NUM_THREADS_PER_GROUP_DIMENSION
        OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
        OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
        OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
    }

};

// This will tell the engine to create the shader and where the shader entry point is.
//                        ShaderType              ShaderPath             Shader function name    Type
IMPLEMENT_GLOBAL_SHADER(FUpscalingShader, "/Gamekit/Upscaling.usf", "MainComputeShader", SF_Compute);


FUpscalingDispatcher* FUpscalingDispatcher::instance = nullptr;

FUpscalingDispatcher* FUpscalingDispatcher::Get()
{
    if (!instance)
        instance = new FUpscalingDispatcher();
    return instance;
};

void FUpscalingDispatcher::BeginRendering()
{
    if (OnPostResolvedSceneColorHandle.IsValid())
    {
        return;
    }
    bCachedParamsAreValid = false;

    const FName RendererModuleName("Renderer");
    IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
    if (RendererModule)
    {
        OnPostResolvedSceneColorHandle = RendererModule->GetResolvedSceneColorCallbacks().AddRaw(this, &FUpscalingDispatcher::Execute_RenderThread);
    }
}

void FUpscalingDispatcher::EndRendering()
{
    if (!OnPostResolvedSceneColorHandle.IsValid())
    {
        return;
    }

    const FName RendererModuleName("Renderer");
    IRendererModule* RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
    if (RendererModule)
    {
        RendererModule->GetResolvedSceneColorCallbacks().Remove(OnPostResolvedSceneColorHandle);
    }

    OnPostResolvedSceneColorHandle.Reset();
}

void FUpscalingDispatcher::UpdateParameters(FUpscalingParameter& Params)
{
    CachedParams          = Params;
    bCachedParamsAreValid = true;
}

void FUpscalingDispatcher::ReserveRenderTargets(FRHICommandListImmediate &RHICmdList)
{
    auto ComputeShaderOutputDesc = FPooledRenderTargetDesc::Create2DDesc(
        CachedParams.OriginalSize * CachedParams.Multiplier,
        // CachedParams.UpscaledTexture->GetRenderTargetResource()->TextureRHI->GetFormat(),
        CachedParams.UpscaledTexture->GetResource()->TextureRHI->GetFormat(),
        FClearValueBinding::Black,
        TexCreate_None,
        TexCreate_ShaderResource | TexCreate_UAV,
        false
    );

    ComputeShaderOutputDesc.DebugName = TEXT("FUpscalingShader_Output_RenderTarget");
    GRenderTargetPool.FindFreeElement(
        RHICmdList, 
        ComputeShaderOutputDesc, 
        ComputeShaderOutput, 
        TEXT("FUpscalingShader_Output_RenderTarget")
    );

    auto ComputeShaderInputDesc =FPooledRenderTargetDesc::Create2DDesc(
        CachedParams.OriginalSize,
        CachedParams.OriginalTexture->GetResource()->TextureRHI->GetFormat(),
        FClearValueBinding::Black,
        TexCreate_None,
        TexCreate_ShaderResource | TexCreate_UAV,
        false
    );

    ComputeShaderInputDesc.DebugName = TEXT("FUpscalingShader_Input_RenderTarget");
    GRenderTargetPool.FindFreeElement(
        RHICmdList, 
        ComputeShaderInputDesc, 
        ComputeShaderInput, 
        TEXT("FUpscalingShader_Input_RenderTarget")
    );
}

void FUpscalingDispatcher::CopyInputTextureToInputTarget(FRHICommandListImmediate &RHICmdList)
{
    // Original Texture is about to be copied from
    RHICmdList.Transition(FRHITransitionInfo(
        CachedParams.OriginalTexture->GetResource()->TextureRHI, 
        ERHIAccess::Unknown,
        ERHIAccess::CopySrc
    ));

    // InputTarget is about to be copied to
    RHICmdList.Transition(FRHITransitionInfo(
        ComputeShaderInput->GetRenderTargetItem().ShaderResourceTexture,
        ERHIAccess::UAVCompute,
        ERHIAccess::CopyDest
    ));

    // Copy the entire target
    RHICmdList.CopyTexture(
        CachedParams.OriginalTexture->GetResource()->TextureRHI,
        ComputeShaderInput->GetRenderTargetItem().ShaderResourceTexture,
        FRHICopyTextureInfo()
    );

    // Set input target back to Compute
    RHICmdList.Transition(FRHITransitionInfo(
        ComputeShaderInput->GetRenderTargetItem().UAV, 
        ERHIAccess::CopyDest, 
        ERHIAccess::UAVCompute
    ));
}


void FUpscalingDispatcher::CopyOutputTargetToOutputTexture(FRHICommandListImmediate &RHICmdList) {
    // Output Target is about to be copied from
    RHICmdList.Transition(FRHITransitionInfo(
        ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture,
        ERHIAccess::UAVCompute,
        ERHIAccess::CopySrc
    ));

    // OutputTexture is about to be copied to
    RHICmdList.Transition(FRHITransitionInfo(
        CachedParams.UpscaledTexture->GetResource()->TextureRHI,
        ERHIAccess::Unknown,
        ERHIAccess::CopyDest)
    ); 

    RHICmdList.CopyTexture(
        ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture,
        // CachedParams.UpscaledTexture->GetRenderTargetResource()->TextureRHI,
        CachedParams.UpscaledTexture->GetResource()->TextureRHI,
        FRHICopyTextureInfo()
    );

    // Set output texture to graphic resource
    RHICmdList.Transition(FRHITransitionInfo(
        CachedParams.UpscaledTexture->GetResource()->TextureRHI,
        ERHIAccess::CopyDest,
        ERHIAccess::SRVGraphics
    ));
}

void FUpscalingDispatcher::Execute_RenderThread(FRHICommandListImmediate& RHICmdList, class FSceneRenderTargets& SceneContext)
{
    if (!(bCachedParamsAreValid && CachedParams.UpscaledTexture))
    {
        UE_LOG(LogGamekit, Warning, TEXT("Skip update, no valid input %d"), bCachedParamsAreValid);
        return;
    }

    check(IsInRenderingThread());

    ReserveRenderTargets(RHICmdList);
    CopyInputTextureToInputTarget(RHICmdList);

    FUpscalingShader::FParameters PassParameters;
    PassParameters.OutputTexture = ComputeShaderOutput->GetRenderTargetItem().UAV;
    PassParameters.InputTexture  = ComputeShaderInput->GetRenderTargetItem().ShaderResourceTexture;
    PassParameters.Dimensions    = CachedParams.OriginalSize;
    PassParameters.TimeStamp     = CachedParams.TimeStamp;
    PassParameters.Multiplier    = CachedParams.Multiplier;

    TShaderMapRef<FUpscalingShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

    FComputeShaderUtils::Dispatch(
        RHICmdList, 
        ComputeShader, 
        PassParameters,
        FIntVector(
            FMath::DivideAndRoundUp(CachedParams.OriginalSize.X, NUM_THREADS_PER_GROUP_DIMENSION),
            FMath::DivideAndRoundUp(CachedParams.OriginalSize.Y, NUM_THREADS_PER_GROUP_DIMENSION), 
            1
        )
    );

    CopyOutputTargetToOutputTexture(RHICmdList);
}

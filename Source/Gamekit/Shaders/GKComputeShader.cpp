#include "Gamekit/Shaders/GKComputeShader.h"

#include "Gamekit.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"

#include "Math/IntVector.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"


#define NUM_THREADS_PER_GROUP_DIMENSION 32

class FUpscalingShader: public FGlobalShader
{
    public:
    DECLARE_EXPORTED_SHADER_TYPE(FUpscalingShader, Global, GAMEKIT_API);
    //DECLARE_GLOBAL_SHADER(FUpscalingShader);
    SHADER_USE_PARAMETER_STRUCT(FUpscalingShader, FGlobalShader);

    BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
        SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, InputTexture)
        SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, OutputTexture)
        SHADER_PARAMETER(FIntVector, OriginalSize)

        //SHADER_PARAMETER_TEXTURE(RWTexture2D<uint>, OutputTexture)
        //SHADER_PARAMETER_TEXTURE(RWTexture2D<uint>, InputTexture)
        //SHADER_PARAMETER_UAV(RWTexture2D<uint>, OutputTexture)
        //SHADER_PARAMETER_UAV(RWTexture2D<uint8>, InputTexture)
        //SHADER_PARAMETER(FIntVector, OriginalSize)
    END_SHADER_PARAMETER_STRUCT()

    public:
    static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters &Parameters)
    {
        return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
    }

    // static bool ShouldCache(EShaderPlatform Platform) { return true; }

    static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters &Parameters,
                                                    FShaderCompilerEnvironment &              OutEnvironment)
    {
        FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
        OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
        OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
        OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
    }
};


/*
	SF_Vertex			= 0,
	SF_Hull				= 1,
	SF_Domain			= 2,
	SF_Pixel			= 3,
	SF_Geometry			= 4,
	SF_Compute			= 5,
	SF_RayGen			= 6,
	SF_RayMiss			= 7,
	SF_RayHitGroup		= 8,
	SF_RayCallable		= 9,
*/
//*
IMPLEMENT_SHADER_TYPE(,
                      FUpscalingShader,
                      TEXT("/Gamekit/Upscaling.usf"),
                      TEXT("UpscalerEntryPoint"),
                      SF_Compute)
//*/


#if CUSTOM_SHADER
FUpscalerShader::FUpscalerShader() { bParamsValid = false; }

void FUpscalerShader::UpdateParameters(FUspcalingShaderParameters &DrawParameters) {
    Parameters   = DrawParameters;
    bParamsValid = true;
}


void FUpscalerShader::CreateOutputTarget(FIntVector Size) {
    FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();

    // Create 2D texture description for writing
    FPooledRenderTargetDesc OutputTargetDesc = FPooledRenderTargetDesc::Create2DDesc(
        FIntPoint(Size.X, Size.Y),
        PF_R8G8B8A8,
        FClearValueBinding::None,
        TexCreate_None,
        TexCreate_UAV | TexCreate_ShaderResource | TexCreate_GenerateMipCapable | TexCreate_RenderTargetable,
        false
    );

    GRenderTargetPool.FindFreeElement(
        RHICmdList,
        OutputTargetDesc,
        OutputTarget,
        TEXT("OutputTarget")
    );
}


void FUpscalerShader::CreateInputTarget(FIntVector Size) {
    FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();

    // Create 2D texture description for reading
    FPooledRenderTargetDesc InputTargetDesc = FPooledRenderTargetDesc::Create2DDesc(
        FIntPoint(Size.X, Size.Y),     // FIntPoint           InExtent
        PF_G16,                        // EPixelFormat        InFormat
        FClearValueBinding::None,      // FCLearValueBinding  InClearValue
        TexCreate_None,                // ETextureCreateFlags InFlags
        TexCreate_ShaderResource,      // ETextureCreateFlags InTargetableFlags
        false                          // bool                bInForceSeparateTargetAndShaderResource
                                       // uint16              InNumMips             = 1
                                       // bool                InAutowritable        = true
                                       // bool                InCreateRTWriteMask   = false
                                       // bool                InCreateFmask         = false
    );

    // Get the Texture
    GRenderTargetPool.FindFreeElement(
        RHICmdList,           // FRHICommandList&                   RHICmdList
        InputTargetDesc,      // const FPooledRenderTargetDesc&     InputDesc
        InputTarget,          // TRefCountPtr<IPooledRenderTarget>& Out
        TEXT("InputTarget")   // const TCHAR*                       InDebugName
                              // ERenderTargetTransience            TransienceHint          = ERenderTargetTransience::Transient
                              // bool                               bDeferTextureAllocation = false
    );
}

void FUpscalerShader::CopyFromUAVToDest(FRHICommandListImmediate &RHICmdList) {
    auto OutputTexture = Parameters.UpscaledTexture->GetResource()->GetTexture2DRHI();

    // Prepare texture to be copied from (i.e writable -> readable)
    RHICmdList.Transition(
        FRHITransitionInfo(
            OutputTarget->GetRenderTargetItem().ShaderResourceTexture,  // class FRHITexture* InTexture
            ERHIAccess::WritableMask,   // WritableMask >               // ERHIAccess InPreviousState
            ERHIAccess::CopySrc                                         // ERHIAccess InNewState
        )
    );

    RHICmdList.Transition(
        FRHITransitionInfo(
            OutputTexture,
            ERHIAccess::Unknown,
            ERHIAccess::CopyDest
        )
    );

    FRHICopyTextureInfo CopyInfo;
    // CopyInfo.Size = OutptSize;
    CopyInfo.SourceMipIndex = 0;
    CopyInfo.DestMipIndex = 0;

    RHICmdList.CopyTexture(
        OutputTarget->GetRenderTargetItem().ShaderResourceTexture,
        OutputTexture,
        CopyInfo
    );

    // Output texture is ready for display
    RHICmdList.Transition(
        FRHITransitionInfo(
            OutputTexture,
            ERHIAccess::CopyDest,
            ERHIAccess::SRVGraphics
        )
    );

    // Back to writable
    RHICmdList.Transition(
        FRHITransitionInfo(
            OutputTarget->GetRenderTargetItem().ShaderResourceTexture,
            ERHIAccess::CopySrc,
            ERHIAccess::WritableMask
        )
    );
}

void FUpscalerShader::CopyFromSrcToSRV(FRHICommandListImmediate &RHICmdList) {
    auto InputTexture = Parameters.OriginalTexture->GetResource()->GetTexture2DRHI();

    // Prepare texture to be copied from (i.e writable -> readable)
    RHICmdList.Transition(
        FRHITransitionInfo(
            InputTexture,           // class FRHITexture* InTexture
            ERHIAccess::Unknown,    // ERHIAccess InPreviousState
            ERHIAccess::CopySrc     // ERHIAccess InNewState
        )
    );

    RHICmdList.Transition(
        FRHITransitionInfo(
            InputTarget->GetRenderTargetItem().ShaderResourceTexture,  
            ERHIAccess::Unknown,       // SRVCompute >                       
            ERHIAccess::CopyDest                                       
        )
    );

    FRHICopyTextureInfo CopyInfo;
    // CopyInfo.Size = OutptSize;
    CopyInfo.SourceMipIndex = 0;
    CopyInfo.DestMipIndex = 0;

    RHICmdList.CopyTexture(
        InputTexture,
        InputTarget->GetRenderTargetItem().ShaderResourceTexture,
        CopyInfo
    );

    // Output texture is ready for compute
    RHICmdList.Transition(
        FRHITransitionInfo(
            InputTarget->GetRenderTargetItem().ShaderResourceTexture,
            ERHIAccess::CopyDest,
            ERHIAccess::SRVCompute
        )
    );
}

void FUpscalerShader::Execute_RenderThread(FRHICommandListImmediate &RHICmdList, class FSceneRenderTargets &SceneContext)
{

    if (!(bParamsValid && Parameters.UpscaledTexture && Parameters.OriginalTexture))
    {
        return;
    }

    QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_ComputeShader); // Used to gather CPU profiling data for the UE4 session frontend
    SCOPED_DRAW_EVENT(RHICmdList, ShaderPlugin_Compute); // Used to profile GPU activity and add metadata to be consumed
                                                         // by for example RenderDoc

    FMemMark Mark(FMemStack::Get());
    FRDGBuilder GraphBuilder(RHICmdList);
    FGPUFenceRHIRef Fence = RHICreateGPUFence(TEXT("UpscaleFence"));

    auto OutptSize = Parameters.TextureSize * 4;

    CreateInputTarget(Parameters.TextureSize);
    CreateOutputTarget(OutptSize); 

    // Move data to our target
    CopyFromSrcToSRV(RHICmdList);

    FRDGTextureRef SrcTexture = GraphBuilder.RegisterExternalTexture(InputTarget);
    FRDGTextureRef DstTexture = GraphBuilder.RegisterExternalTexture(OutputTarget);

    FRDGTextureUAVRef UAV = GraphBuilder.CreateUAV(DstTexture);
    FRDGTextureSRVRef SRV = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(SrcTexture));;

    TShaderMapRef<FUpscalingShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

    FUpscalingShader::FParameters* Params = GraphBuilder.AllocParameters<FUpscalingShader::FParameters>();
    Params->InputTexture = SRV;
    Params->OutputTexture = UAV;
    Params->OriginalSize = Parameters.TextureSize;

    const FIntVector GroupCount(
        FMath::DivideAndRoundUp(Parameters.TextureSize.X, NUM_THREADS_PER_GROUP_DIMENSION),
        FMath::DivideAndRoundUp(Parameters.TextureSize.Y, NUM_THREADS_PER_GROUP_DIMENSION),
        1
    );

    FComputeShaderUtils::AddPass(
        GraphBuilder,
        RDG_EVENT_NAME("ShaderTypePass"),
        ComputeShader,
        Params,
        GroupCount
    );

    GraphBuilder.Execute();

    CopyFromUAVToDest(RHICmdList);

    // Wait for everything to finish
    RHICmdList.WriteGPUFence(Fence);

    // Release all the resources
    Fence.SafeRelease();
    InputTarget.SafeRelease();
    OutputTarget.SafeRelease();
}


FUpscalerShader *FUpscalerShader::instance = nullptr;


void FUpscalerShader::BeginRendering()
{
    // If the handle is already initalized and valid, no need to do anything
    if (OnPostResolvedSceneColorHandle.IsValid())
    {
        return;
    }
    bParamsValid = false;
    // Get the Renderer Module and add our entry to the callbacks so it can be executed each frame after the scene
    // rendering is done
    const FName      RendererModuleName("Renderer");
    IRendererModule *RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
    if (RendererModule)
    {
        OnPostResolvedSceneColorHandle = RendererModule->GetResolvedSceneColorCallbacks().AddRaw(
            this, &FUpscalerShader::Execute_RenderThread);
    }
}

// Stop the compute shader execution
void FUpscalerShader::EndRendering()
{
    // If the handle is not valid then there's no cleanup to do
    if (!OnPostResolvedSceneColorHandle.IsValid())
    {
        return;
    }

    // Get the Renderer Module and remove our entry from the ResolvedSceneColorCallbacks
    const FName      RendererModuleName("Renderer");
    IRendererModule *RendererModule = FModuleManager::GetModulePtr<IRendererModule>(RendererModuleName);
    if (RendererModule)
    {
        RendererModule->GetResolvedSceneColorCallbacks().Remove(OnPostResolvedSceneColorHandle);
    }

    OnPostResolvedSceneColorHandle.Reset();
}

FUpscalerShader *FUpscalerShader::Get()
{
    if (!instance)
        instance = new FUpscalerShader();

    return instance;
};

#endif

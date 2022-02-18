#include "Gamekit.h"
#include "Gamekit/Shaders/GKComputeShader.h"

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
        SHADER_PARAMETER_TEXTURE(RWTexture2D<uint>, OutputTexture)
        SHADER_PARAMETER_TEXTURE(RWTexture2D<uint>, InputTexture)
        //SHADER_PARAMETER_UAV(RWTexture2D<uint>, OutputTexture)
        //SHADER_PARAMETER_UAV(RWTexture2D<uint8>, InputTexture)
        SHADER_PARAMETER(FIntVector, OriginalSize)
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

void FUpscalerShader::Execute_RenderThread(FRHICommandListImmediate &RHICmdList, class FSceneRenderTargets &SceneContext)
{

    if (!(bParamsValid && Parameters.UpscaledTexture && Parameters.OriginalTexture))
    {
        return;
    }

    QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_ComputeShader); // Used to gather CPU profiling data for the UE4 session frontend
    SCOPED_DRAW_EVENT(RHICmdList, ShaderPlugin_Compute); // Used to profile GPU activity and add metadata to be consumed
                                                         // by for example RenderDoc

    /*
    if (!ComputeShaderOutput.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Upscaler_Output_RenderTarget is not valid, creating"));
        FPooledRenderTargetDesc ComputeShaderOutputDesc(FPooledRenderTargetDesc::Create2DDesc(
                Parameters.GetUpscaleSize(),
                Parameters.UpscaledTexture->GetRenderTargetResource()->TextureRHI->GetFormat(),
                FClearValueBinding::None,
                // TexCreate_Dynamic | TexCreate_CPUWritable | TexCreate_RenderTargetable
                TexCreate_None,
                TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_Dynamic,
                false));

        ComputeShaderOutputDesc.DebugName = TEXT("Upscaler_Output_RenderTarget");
        GRenderTargetPool.FindFreeElement(
                RHICmdList, ComputeShaderOutputDesc, ComputeShaderOutput, TEXT("Upscaler_Output_RenderTarget"));
    }

    if (!ComputeShaderInput.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("Upscaler_Input_RenderTarget is not valid, creating"));
        FPooledRenderTargetDesc ComputeShaderInputDesc(FPooledRenderTargetDesc::Create2DDesc(
                Parameters.GetUpscaleSize(),
                Parameters.UpscaledTexture->GetRenderTargetResource()->TextureRHI->GetFormat(),
                FClearValueBinding::None,
                // TexCreate_Dynamic | TexCreate_CPUWritable | TexCreate_RenderTargetable
                TexCreate_None,
                TexCreate_ShaderResource | TexCreate_RenderTargetable | TexCreate_Dynamic,
                false));

        ComputeShaderInputDesc.DebugName = TEXT("Upscaler_Input_RenderTarget");
        GRenderTargetPool.FindFreeElement(
                RHICmdList, ComputeShaderInputDesc, ComputeShaderInput, TEXT("Upscaler_Input_RenderTarget"));
    }

   
    FRHITransition(ERHIPipeline::Graphics, ERHIPipeline::AsyncCompute)
    RHICmdList.TransitionResources()
    RHICmdList.TransitionResource(EResourceTransitionAccess::ERWBarrier,
                                  EResourceTransitionPipeline::EGfxToCompute,
                                  ComputeShaderOutput->GetRenderTargetItem().UAV);

    RHICmdList.TransitionResource(EResourceTransitionAccess::ERWBarrier,
                                  EResourceTransitionPipeline::EGfxToCompute,
                                  ComputeShaderInput->GetRenderTargetItem().UAV);
    */

    // ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture;
    // ComputeShaderOutput->GetRenderTargetItem().TargetableTexture;

    /*
    RHICmdList.CopyTexture(
        Parameters.OriginalTexture->GetResource()->GetTexture2DRHI(),
        ComputeShaderInput->GetRenderTargetItem().ShaderResourceTexture,
        FRHICopyTextureInfo());
    */

    FUpscalingShader::FParameters PassParameters;
    // PassParameters.OutputTexture = ComputeShaderOutput->GetRenderTargetItem().TargetableTexture;
    
    FRHIRenderPassInfo RenderPassInfo(Parameters.UpscaledTexture->GetRenderTargetResource()->GetRenderTargetTexture(),
                                      ERenderTargetActions::Clear_Store);
    RHICmdList.BeginRenderPass(RenderPassInfo, TEXT("ShaderPlugin_OutputToRenderTarget"));

    PassParameters.OutputTexture = Parameters.UpscaledTexture->GetResource()->GetTexture2DRHI();
    PassParameters.InputTexture  = Parameters.OriginalTexture->GetResource()->GetTexture2DRHI();
    PassParameters.OriginalSize  = Parameters.TextureSize;


    TShaderMapRef<FUpscalingShader> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
    // SetShaderParameters(RHICmdList, ComputeShader, ComputeShader-(), PassParameters);

    //* 
    FComputeShaderUtils::Dispatch(
            RHICmdList,
            ComputeShader,
            PassParameters,
            FIntVector(
                FMath::DivideAndRoundUp(Parameters.TextureSize.X, NUM_THREADS_PER_GROUP_DIMENSION),
                FMath::DivideAndRoundUp(Parameters.TextureSize.Y, NUM_THREADS_PER_GROUP_DIMENSION),
                1
            )
    );
    //*/

    RHICmdList.CopyToResolveTarget(Parameters.UpscaledTexture->GetRenderTargetResource()->GetRenderTargetTexture(),
                                   Parameters.UpscaledTexture->GetRenderTargetResource()->TextureRHI,
                                   FResolveParams());

    RHICmdList.EndRenderPass();

    /*/ Copy shader's output to the render target provided by the client
    RHICmdList.CopyTexture(
        ComputeShaderOutput->GetRenderTargetItem().TargetableTexture,
        Parameters.UpscaledTexture->GetRenderTargetResource()->TextureRHI,
        FRHICopyTextureInfo()
    );
    */
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

#pragma once

#include "Gamekit.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"

//This struct act as a container for all the parameters that the client needs to pass to the Compute Shader Manager.
struct FUpscalingParameter
{
public:
    class UTexture2D *OriginalTexture;
	class UTexture2D *UpscaledTexture;
	// class UCanvasRenderTarget2D *UpscaledTexture;
    FIntPoint         OriginalSize;
    uint32            TimeStamp;
    uint32            Multiplier;
};


/// <summary>
/// A singleton Shader Manager for our Shader Type
/// </summary>
class GAMEKIT_API FUpscalingDispatcher
{
public:
	//Get the instance
	static FUpscalingDispatcher* Get();

	// Call this when you want to hook onto the renderer and start executing the compute shader. The shader will be dispatched once per frame.
	void BeginRendering();

	// Stops compute shader execution
	void EndRendering();

	// Call this whenever you have new parameters to share.
	void UpdateParameters(FUpscalingParameter& DrawParameters);
	
private:
	//Private constructor to prevent client from instanciating
	FUpscalingDispatcher() = default;

	//The singleton instance
	static FUpscalingDispatcher* instance;

	//The delegate handle to our function that will be executed each frame by the renderer
	FDelegateHandle OnPostResolvedSceneColorHandle;

	//Cached Shader Manager Parameters
	FUpscalingParameter CachedParams;

	//Whether we have cached parameters to pass to the shader or not
	volatile bool bCachedParamsAreValid;

	//Reference to a pooled render target where the shader will write its output
	TRefCountPtr<IPooledRenderTarget> ComputeShaderOutput;
	TRefCountPtr<IPooledRenderTarget> ComputeShaderInput;

	void ReserveRenderTargets(FRHICommandListImmediate &RHICmdList);
    void CopyInputTextureToInputTarget(FRHICommandListImmediate &RHICmdList);
    void CopyOutputTargetToOutputTexture(FRHICommandListImmediate &RHICmdList);

public:
	void Execute_RenderThread(FRHICommandListImmediate& RHICmdList, class FSceneRenderTargets& SceneContext);
};

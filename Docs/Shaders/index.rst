E:\UnrealEngine\Engine\Source\Runtime\RenderCore\Public\ShaderParameterMacros.h


/** Adds a shader resource view for a render graph tracked texture.


SHADER_PARAMETER_RDG_TEXTURE
SHADER_PARAMETER_SAMPLER
SHADER_PARAMETER_TEXTURE
RENDER_TARGET_BINDING_SLOTS
SHADER_PARAMETER_SRV(Buffer<float>
SHADER_PARAMETER(FVector, Scale)
SHADER_PARAMETER(float, WhiteTemp)
SHADER_PARAMETER_UAV(RWStructuredBuffer<FIrradianceCacheRecord>, IrradianceCacheRecords)
SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
SHADER_PARAMETER_STRUCT_REF(FSceneTextureUniformParameters, SceneTextures)
SHADER_PARAMETER_STRUCT_REF(FIrradianceCachingParameters, IrradianceCachingParameters)
SHADER_PARAMETER_UAV(RWTexture2D<float4>, ScratchTilePoolLayer0)
SHADER_PARAMETER_RDG_TEXTURE(Texture2D, LightGrid)
SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<uint>, LightGridData)
SHADER_PARAMETER_UAV(RWTexture3D<float3>, AmbientVector)
SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, IrradianceAndSampleCount)




	LAYOUT_FIELD(FShaderResourceParameter, InputTexture)
	LAYOUT_FIELD(FShaderResourceParameter, InputTextureSampler)

	LAYOUT_FIELD(FShaderResourceParameter, OCIO3dTexture)
	LAYOUT_FIELD(FShaderResourceParameter, OCIO3dTextureSampler)
	LAYOUT_FIELD(FShaderParameter, Gamma)

private:
	LAYOUT_FIELD(FMemoryImageString, DebugDescription)




    FOpenColorIOPixelShader


/** Shader for MinMax downsample passes. */
class FMinMaxTextureCS : public FGlobalShader
{
public:
	SHADER_USE_PARAMETER_STRUCT(FMinMaxTextureCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, SrcTexture)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, DstTexture)
		SHADER_PARAMETER(FIntPoint, SrcTextureSize)
		SHADER_PARAMETER(FIntPoint, DstTextureCoord)
	END_SHADER_PARAMETER_STRUCT()
};

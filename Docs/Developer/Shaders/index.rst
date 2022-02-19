Shaders
=======

Unreal Shading language is HLSL.


* ``.ush``: Unreal Shader Headers

  * included by other USH or USF files

* ``.usf``: Unreal Shader Format

  * should be private data only
  * should contain shader entry points i.e custom shaders.


Write HLSL functions for your materials
---------------------------------------

To write shader code directly, you can add a custom shader path in your project module.

.. code-block:: cpp

	// Maps ``/Gamekit`` to ``/path/to/project/Gamekit/Shaders``
	void FGamekitModule::StartupModule() {
			// GamekitShaders
			FString ShaderDirectory = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("<ProjectFolderName>"), TEXT("<ShaderFolder>>"));

			// Make sure the mapping does not exist before adding it
			if (!AllShaderSourceDirectoryMappings().Contains("/<ShaderFolderShortcut>")){
				AddShaderSourceDirectoryMapping("/<ShaderFolderShortcut>", ShaderDirectory);
			}
	}

	void FGamekitModule::ShutdownModule() {
			ResetAllShaderSourceDirectoryMappings();
	}


To include a Gamekit shader file simply add ``/<ShaderFolderShortcut>/<ShaderFile: fire>.ush``

Examples
~~~~~~~~

Gaussian Blur
^^^^^^^^^^^^^

.. code-block:: cpp

	SamplerState TexSampler;

	//! Simple 3x3 Gaussian Kernel
	float3 GaussianBlur(Texture2D Tex, float2 UV, float Distance) {

		float3 newSample =
			Texture2DSample(Tex, TexSampler, UV + float2(-1,  1) * Distance) * 1.f +
			Texture2DSample(Tex, TexSampler, UV + float2( 0,  1) * Distance) * 2.f +
			Texture2DSample(Tex, TexSampler, UV + float2( 1,  1) * Distance) * 1.f +
			Texture2DSample(Tex, TexSampler, UV + float2(-1,  0) * Distance) * 2.f +
			Texture2DSample(Tex, TexSampler, UV + float2( 0,  0) * Distance) * 4.f +
			Texture2DSample(Tex, TexSampler, UV + float2( 1,  0) * Distance) * 2.f +
			Texture2DSample(Tex, TexSampler, UV + float2(-1, -1) * Distance) * 1.f +
			Texture2DSample(Tex, TexSampler, UV + float2( 0, -1) * Distance) * 2.f +
			Texture2DSample(Tex, TexSampler, UV + float2( 1, -1) * Distance) * 1.f;

		return newSample / 16.f;
	}

Custom Shaders
--------------

To follow this part you will need to do the steps described in the previous section.


Global Shaders
~~~~~~~~~~~~~~


Compute Shaders
~~~~~~~~~~~~~~~


Shader Parameters
~~~~~~~~~~~~~~~~~

* class FRDGTexture;
* class FRDGTextureSRV;
* class FRDGTextureUAV;
* class FRDGBuffer;
* class FRDGBufferSRV;
* class FRDGBufferUAV;
* class FRDGUniformBuffer;

* template <typename TUniformStruct> class TRDGUniformBuffer;
* SHADER_PARAMETER_ARRAY(float, MyScalarArray, [8])
* SHADER_PARAMETER_TEXTURE(Texture2D, MyTexture)
* SHADER_PARAMETER_SRV(Texture2D, MySRV): Shader Resource View
* SHADER_PARAMETER_UAV(Texture2D, MyUAV): unordered access view.
* SHADER_PARAMETER_SAMPLER(SamplerState, MySampler)
* SHADER_PARAMETER_RDG_TEXTURE(Texture2D, MyTexture)
* SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, MySRV): shader resource view for a render graph tracked texture
* SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, MyUAV)
* SHADER_PARAMETER_RDG_BUFFER(Buffer<float4>, MyBuffer)
* SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, MySRV)
* SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, MyUAV)
* SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FMyStruct, MemberName)
* RDG_BUFFER_ACCESS(MyBuffer)
* RDG_BUFFER_ACCESS_DYNAMIC
* RDG_TEXTURE_ACCESS
* RDG_TEXTURE_ACCESS_DYNAMIC
* SHADER_PARAMETER_RDG_BUFFER_UPLOAD

Reference
---------

.. [1] `Custom Shaders <https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Rendering/ShaderInPlugin/Overview/>`_
.. [2] `Add shader extensions to VSCode <https://stackoverflow.com/questions/29973619/how-to-make-vs-code-treat-a-file-extensions-as-a-certain-language/51228725#51228725>`_
.. [3] `ShaderParameterMacros <https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/RenderCore/Public/ShaderParameterMacros.h>`_
.. [4] `HLSL Data types <https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-data-types>`_


.. code-block:: cpp

	/** Adds a shader resource view for a render graph tracked texture.


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

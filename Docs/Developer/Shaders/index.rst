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

Custom Shaders
~~~~~~~~~~~~~~

Implement your Shader
^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    /** Shader for MinMax downsample passes. */
    class FMinMaxTextureCS : public FGlobalShader
    {
    public:
        DECLARE_SHADER_TYPE(FMinMaxTextureCS, Global);

        SHADER_USE_PARAMETER_STRUCT(FMinMaxTextureCS, FGlobalShader);

        BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
            SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, SrcTexture)
            SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, DstTexture)
            SHADER_PARAMETER(FIntPoint, SrcTextureSize)
            SHADER_PARAMETER(FIntPoint, DstTextureCoord)
        END_SHADER_PARAMETER_STRUCT()

        static bool ShouldCompilePermutation(FGlobalShaderPermutationParameters const& Parameters)
        {
            return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
        }

        static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
        {
            FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
        }
    };

.. code-block:: cpp

    IMPLEMENT_SHADER_TYPE(,
                                 FMinMaxTextureCS,
                                 TEXT("/Plugin/VirtualHeightfieldMesh/Private/HeightfieldMinMaxRender.usf"),
                                 TEXT("MinMaxHeightCS"),
                                 SF_Compute);

.. code-block:: cpp

    void GenerateMinMaxTextureMips(FRDGBuilder& GraphBuilder, FRDGTexture* Texture, FIntPoint SrcSize, int32 NumMips)
    {
        FIntPoint Size = SrcSize;
        for (int32 MipLevel = 1; MipLevel < NumMips; ++MipLevel)
        {
            FRDGTextureSRVRef SRV = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::CreateForMipLevel(Texture, MipLevel - 1));
            FRDGTextureUAVRef UAV = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(Texture, MipLevel));

            AddMinMaxMipPass<TMinMaxTextureCS_RGBA8ToRGBA8>(GraphBuilder, SRV, Size, MipLevel, UAV);

            Size.X = FMath::Max(Size.X / 2, 1);
            Size.Y = FMath::Max(Size.Y / 2, 1);
        }
    }

    void AddMinMaxMipPass(FRDGBuilder& GraphBuilder, FRDGTextureSRVRef Src, FIntPoint SrcSize, int32 SrcMipLevel, FRDGTextureUAVRef Dst)
    {
        TShaderMapRef<ShaderType> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

        FMinMaxTextureCS::FParameters* Parameters = GraphBuilder.AllocParameters<FMinMaxTextureCS::FParameters>();
        Parameters->SrcTexture = Src;
        Parameters->DstTexture = Dst;
        Parameters->SrcTextureSize = SrcSize;

        const FIntVector GroupCount((SrcSize.X / 2 + 7) / 8, (SrcSize.Y / 2 + 7) / 8, 1);

        ClearUnusedGraphResources(ComputeShader, Parameters);

        GraphBuilder.AddPass(
            RDG_EVENT_NAME("MinMaxPass"),
            Parameters,
            ERDGPassFlags::Compute,
            [Parameters, ComputeShader, GroupCount](FRHICommandList& RHICmdList)
            {
                FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *Parameters, GroupCount);
            });
    }

Create Textures for your shader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

See ``UnrealEngine\Engine\Source\Runtime\RHI\Public\RHIDefinitions.h`` for the ``ETextureCreateFlags`` enum.


Use High level Targets
++++++++++++++++++++++

.. code-block:: cpp

    auto Texture = UTexture2D::CreateTransient(
        Buffer.Width(),
        Buffer.Height(),
        EPixelFormat::PF_G8
    );

    Texture->CompressionSettings = TextureCompressionSettings::TC_Grayscale;
    Texture->SRGB                = false;
    Texture->Filter              = TextureFilter::TF_Nearest;
    Texture->AddressX            = TextureAddress::TA_Clamp;
    Texture->AddressY            = TextureAddress::TA_Clamp;
    Texture->MipGenSettings      = TextureMipGenSettings::TMGS_NoMipmaps;
    Texture->UpdateResource();

    FRHITexture2D* RHITexture = Texture->GetResource()->GetTexture2DRHI();


.. code-block:: cpp

    auto Texture =  UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
        GetWorld(),
        UCanvasRenderTarget2D::StaticClass(),
        TextureSize.X,
        TextureSize.Y);

    FRHITexture2D* RHITexture = Texture->GetResource()->GetTexture2DRHI();


Create New RHI Target
+++++++++++++++++++++

Input Target
````````````

.. code-block:: cpp

    FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();

    // Create 2D texture description for reading
    FPooledRenderTargetDesc InputTargetDesc = FPooledRenderTargetDesc::Create2DDesc(
        FIntPoint(TileSize, TileSize), // FIntPoint           InExtent
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
    TRefCountPtr<IPooledRenderTarget> InputTarget;
    GRenderTargetPool.FindFreeElement(
        RHICmdList,           // FRHICommandList&                   RHICmdList
        InputTargetDesc,      // const FPooledRenderTargetDesc&     InputDesc
        InputTarget,          // TRefCountPtr<IPooledRenderTarget>& Out
        TEXT("InputTarget")   // const TCHAR*                       InDebugName
                              // ERenderTargetTransience            TransienceHint          = ERenderTargetTransience::Transient
                              // bool                               bDeferTextureAllocation = false
    );


Output Target
`````````````

.. code-block:: cpp

    // Create 2D texture description for writing
    FPooledRenderTargetDesc OutputTargetDesc = FPooledRenderTargetDesc::Create2DDesc(
        FIntPoint(NumTilesX, NumTilesY),
        PF_R8G8B8A8,
        FClearValueBinding::None,
        TexCreate_None,
        TexCreate_UAV | TexCreate_ShaderResource | TexCreate_GenerateMipCapable | TexCreate_RenderTargetable,
        false,
    );

    TRefCountPtr<IPooledRenderTarget> OutputTarget;
    GRenderTargetPool.FindFreeElement(
        RHICmdList,
        OutputTargetDesc,
        OutputTarget,
        TEXT("OutputTarget")
    );



Schedule your shader for execution
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block::

    // Downsample page to texel in output
    FMemMark Mark(FMemStack::Get());
    FRDGBuilder GraphBuilder(RHICmdList);

    FRDGTextureRef SrcTexture = GraphBuilder.RegisterExternalTexture(InputTarget);
    FRDGTextureRef DstTexture = GraphBuilder.RegisterExternalTexture(OutputTarget);

    FRDGTextureUAVRef UAV = GraphBuilder.CreateUAV(DstTexture);
    FRDGTextureSRVRef SRV = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(SrcTexture));;

    TShaderMapRef<ShaderType> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

    FMinMaxTextureCS::FParameters* Parameters = GraphBuilder.AllocParameters<ShaderType::FParameters>();
    Parameters->SrcTexture = SRV;
    Parameters->DstTexture = UAV;
    Parameters->SrcTextureSize = SrcSize;

    const FIntVector GroupCount((SrcSize.X / 2 + 7) / 8, (SrcSize.Y / 2 + 7) / 8, 1);

    FComputeShaderUtils::AddPass(
        GraphBuilder,
        RDG_EVENT_NAME("ShaderTypePass"),
        ComputeShader,
        Parameters,
        GroupCount
    );

    GraphBuilder.Execute();









.. code-block::

    FMemMark Mark(FMemStack::Get());
    FRDGBuilder GraphBuilder(RHICmdList);

    FRDGTextureRef Texture = GraphBuilder.RegisterExternalTexture(
        OutputTarget
    );

    FRDGTextureSRVRef SRV = GraphBuilder.CreateSRV(FRDGTextureSRVDesc::Create(Texture));
    FRDGTextureUAVRef UAV = GraphBuilder.CreateUAV(FRDGTextureUAVDesc(Texture, MipLevel));

    TShaderMapRef<ShaderType> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

    FMinMaxTextureCS::FParameters* Parameters = GraphBuilder.AllocParameters<FMinMaxTextureCS::FParameters>();
    Parameters->SrcTexture = SRV;
    Parameters->DstTexture = UAV;
    Parameters->SrcTextureSize = SrcSize;

    const FIntVector GroupCount((SrcSize.X / 2 + 7) / 8, (SrcSize.Y / 2 + 7) / 8, 1);

    ClearUnusedGraphResources(ComputeShader, Parameters);

    GraphBuilder.AddPass(
        RDG_EVENT_NAME("MyComputeShader"),
        Parameters,
        ERDGPassFlags::Compute,
        [Parameters, ComputeShader, GroupCount](FRHICommandList& RHICmdList)
        {
            FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *Parameters, GroupCount);
        }
    );

    GraphBuilder.Execute();

    FGPUFenceRHIRef Fence = RHICreateGPUFence(TEXT("CopyFence"));

    // Prepare texture to be copied from (i.e writable -> readable)
    RHICmdList.Transition(
        FRHITransitionInfo(
            OutputTarget->GetRenderTargetItem().ShaderResourceTexture,  // class FRHITexture* InTexture
            ERHIAccess::WritableMask,                                   // ERHIAccess InPreviousState
            ERHIAccess::CopySrc                                         // ERHIAccess InNewState
        )
    );

    // Prepare texture to be written
    RHICmdList.Transition(
        FRHITransitionInfo(
            RenderTileResources.GetStagingTexture(MipLevel),
            ERHIAccess::Unknown,
            ERHIAccess::CopyDest
        )
    );

    // Copy
    FRHICopyTextureInfo CopyInfo;
    CopyInfo.Size = FIntVector(RenderTileResources.GetStagingTexture(MipLevel)->GetSizeXYZ());
    CopyInfo.SourceMipIndex = 0;
    CopyInfo.DestMipIndex = 0;

    RHICmdList.CopyTexture(
        RenderTileResources.GetFinalRenderTarget()->GetRenderTargetItem().ShaderResourceTexture,
        RenderTileResources.GetStagingTexture(MipLevel),
        CopyInfo
    );

    // Wait for the copy ?
    // RHICmdList.WriteGPUFence(Fence);



Retrieve result
^^^^^^^^^^^^^^^



.. comment::

   StagingTextures.Add(RHICreateTexture2D(SizeX, SizeY, PF_R8G8B8A8, 1, 1, TexCreate_CPUReadback, CreateInfo));
   FGPUFenceRHIRef Fence = RHICreateGPUFence(TEXT("Runtime Virtual Texture Build"));


Use Custom Shaders as materials
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Global Shaders
~~~~~~~~~~~~~~

.. warning::

    Shader compiler error will make the editor crash during load up


.. warning::

    Module using global shaders needs to be configured to load during the ``PostConfigInit`` phase.
    If not the loading will crash with a criptic error message about the OS not being able to load
    your library.

    .. code-block:: json

        "Modules": [
            {
                "Name": "Gamekit",
                "Type": "Runtime",
                "LoadingPhase": "PostConfigInit",
                "WhitelistPlatforms": [ "Win64" ]
            }
        ],


Compute Shaders
~~~~~~~~~~~~~~~

You can use a regular shader to do compute.
The main advantage of using compute shader instead would be to decouple the computation from the Rendering
pipeline, but then you will need to synchronise with it.


Shader Parameters
~~~~~~~~~~~~~~~~~

Common abbreviation:

* RDG: render graph
* UAV: unordered access view
* SRV: Shader Resource View


Parameters:

* SHADER_PARAMETER_ARRAY(float, MyScalarArray, [8])
* SHADER_PARAMETER_TEXTURE(Texture2D, MyTexture)
* SHADER_PARAMETER_SRV(Texture2D, MySRV)
* SHADER_PARAMETER_UAV(Texture2D, MyUAV)
* SHADER_PARAMETER_SAMPLER(SamplerState, MySampler)
* SHADER_PARAMETER_RDG_TEXTURE(Texture2D, MyTexture)
* SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D, MySRV)
* SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D, MyUAV)
* SHADER_PARAMETER_RDG_BUFFER(Buffer<float4>, MyBuffer)
* SHADER_PARAMETER_RDG_BUFFER_SRV(Buffer<float4>, MySRV)
* SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<float4>, MyUAV)
* SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FMyStruct, MemberName)
* SHADER_PARAMETER_RDG_BUFFER_UPLOAD

.. node::

    Most of the parameters have an array version ``SHADER_PARAMETER_RDG_TEXTURE_ARRAY``


* RDG_BUFFER_ACCESS(MyBuffer)
* RDG_BUFFER_ACCESS_DYNAMIC
* RDG_TEXTURE_ACCESS
* RDG_TEXTURE_ACCESS_DYNAMIC


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

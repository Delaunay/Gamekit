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

Implement your Shader
^^^^^^^^^^^^^^^^^^^^^

.. warning::

    Global Shader compile error will make the editor crash during load up

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


.. code-block:: cpp

    class FUpscalingShader : public FGlobalShader
    {
    public:
        DECLARE_GLOBAL_SHADER(FUpscalingShader);
        SHADER_USE_PARAMETER_STRUCT(FUpscalingShader, FGlobalShader);

        BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
            SHADER_PARAMETER_TEXTURE(Texture2D<uint>, InputTexture)  // Read Only Texture
            SHADER_PARAMETER_UAV(RWTexture2D<uint>, OutputTexture)   // Read Write Texture
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

            // Preprocessor defines
            OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), NUM_THREADS_PER_GROUP_DIMENSION);
            OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), NUM_THREADS_PER_GROUP_DIMENSION);
            OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
        }
    };

   IMPLEMENT_GLOBAL_SHADER(
       FUpscalingShader,            // Shader Type
       "/Gamekit/Upscaling.usf",    // Shader File
       "MainComputeShader",         // Shader Entry point
       SF_Compute                   // Shader Kind (Vertex, Hull, Domain, Pixel, Geometry, Compute, RayGen, RayMiss, RayHitGroup, RayCallable)
    );

Scheduling
^^^^^^^^^^

.. code-block:: cpp

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


Create Textures for your shader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

See ``UnrealEngine\Engine\Source\Runtime\RHI\Public\RHIDefinitions.h`` for the ``ETextureCreateFlags`` enum.


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

Initialize Input Target
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

    // Copy your input texture to the target texture
    RHICmdList.CopyTexture(
        GetTextureRHI(CachedParams.OriginalTexture),
        ComputeShaderInput->GetRenderTargetItem().ShaderResourceTexture,
        FRHICopyTextureInfo()
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
        TexCreate_UAV,
        false,
    );

    TRefCountPtr<IPooledRenderTarget> OutputTarget;
    GRenderTargetPool.FindFreeElement(
        RHICmdList,
        OutputTargetDesc,
        OutputTarget,
        TEXT("OutputTarget")
    );


Retrive Output
~~~~~~~~~~~~~~

.. code-block:: cpp

    // Copy the output target to the output texture
    RHICmdList.CopyTexture(
        ComputeShaderOutput->GetRenderTargetItem().ShaderResourceTexture,
         GetTextureRHI(CachedParams.UpscaledTexture),
        FRHICopyTextureInfo()
    );

Use Custom Shaders as materials
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. todo::

   Finish that section


Shader Parameters
-----------------

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


Common Errors
-------------


.. code-block:: bash

    [2022.02.19-18.32.59:344][522]LogWindows: Windows GetLastError: The operation completed successfully. (0)
    [2022.02.19-18.32.59:345][522]LogWindows: Error: === Critical error: ===
    [2022.02.19-18.32.59:345][522]LogWindows: Error:
    [2022.02.19-18.32.59:345][522]LogWindows: Error: Fatal error: [File:C:/opt/UnrealEngine/Engine/Source/Runtime/RenderCore/Private/RenderingThread.cpp] [Line: 902]
    [2022.02.19-18.32.59:345][522]LogWindows: Error: Rendering thread exception:
    [2022.02.19-18.32.59:345][522]LogWindows: Error: Assertion failed: !GRDGInExecutePassScope [File:C:/opt/UnrealEngine/Engine/Source/Runtime/RenderCore/Private/RenderGraphValidation.cpp] [Line: 512]
    [2022.02.19-18.32.59:345][522]LogWindows: Error: Render graph is being executed recursively. This usually means a separate FRDGBuilder instance was created inside of an executing pass.


Reference
---------

.. [1] `Custom Shaders <https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Rendering/ShaderInPlugin/Overview/>`_
.. [2] `Add shader extensions to VSCode <https://stackoverflow.com/questions/29973619/how-to-make-vs-code-treat-a-file-extensions-as-a-certain-language/51228725#51228725>`_
.. [3] `ShaderParameterMacros <https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/RenderCore/Public/ShaderParameterMacros.h>`_
.. [4] `HLSL Data types <https://docs.microsoft.com/en-us/windows/win32/direct3dhlsl/dx-graphics-hlsl-data-types>`_
.. [5] `Compute Shaders <https://medium.com/realities-io/using-compute-shaders-in-unreal-engine-4-f64bac65a907>`_

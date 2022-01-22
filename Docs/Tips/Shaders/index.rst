Shader Materials
================

To include a Gamekit shader file simply add ``/Gamekit/<ShaderFile: fire>.usf``

Unreal Shading language is mostly HLSL.


* ``.ush``: Unreal Shader Headers

    * included by other USH or USF files 

* ``.usf``: Unreal Shader Format

    * should be private data only
    * should contain shader entry points 


References
----------

* `Custom Shaders <https://docs.unrealengine.com/4.27/en-US/ProgrammingAndScripting/Rendering/ShaderInPlugin/Overview/>`_
* `Add shader extensions to VSCode <https://stackoverflow.com/questions/29973619/how-to-make-vs-code-treat-a-file-extensions-as-a-certain-language/51228725#51228725>`_



Gaussian Blur
~~~~~~~~~~~~~

.. code-block:: cpp

   float3 newSample =
        Texture2DSample(Tex,TexSampler,UV + float2(-1,  1) * Distance) * 1.f +
        Texture2DSample(Tex,TexSampler,UV + float2( 0,  1) * Distance) * 2.f +
        Texture2DSample(Tex,TexSampler,UV + float2( 1,  1) * Distance) * 1.f +
        Texture2DSample(Tex,TexSampler,UV + float2(-1,  0) * Distance) * 2.f +
        Texture2DSample(Tex,TexSampler,UV + float2( 0,  0) * Distance) * 4.f +
        Texture2DSample(Tex,TexSampler,UV + float2( 1,  0) * Distance) * 2.f +
        Texture2DSample(Tex,TexSampler,UV + float2(-1, -1) * Distance) * 1.f +
        Texture2DSample(Tex,TexSampler,UV + float2( 0, -1) * Distance) * 2.f +
        Texture2DSample(Tex,TexSampler,UV + float2( 1, -1) * Distance) * 1.f;

   return newSample / 16.f;

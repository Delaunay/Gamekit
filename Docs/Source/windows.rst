Compile From Source
===================


* Install build tool

    * windows SDK (which one?)
    * .NET dev+runtime
    * C++ x64 Native

.. note::

    From `2022 build tools <https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022>`_ 
    the compiler and the build tools can be installed without visual studio.


.. code-block::bash

   Setup.bat
   GenerateProjectFiles.bat
   msbuild UE4.sln -p:Configuration=DebugGame -p:Platform=Win64 -m 


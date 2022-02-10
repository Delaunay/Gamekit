Compilation
===========

.. warning::

   You will need upward of 130 Go of space to compile UE4 with debug symbols


.. code-block:: bash

     48   M	./Build
    228   M	./Documentation
     22   G	./Source                <= Because Source/ThirdParty include 21G of binaries
      4.1 G	./Extras
    692   K	./Config
     38   M	./Programs
     21   G	./Binaries              <= Compiled Binaries will vary
      7.1 M	./Shaders
      1.7 G	./Content
    483   M	./DerivedDataCache
     31   G	./Plugins               <= Include the compiled binaries of each plugin
     41   G	./Intermediate          <= Intermediate/Build for incremental builds
      5.4 M	./Saved
    120   G	.

Windows
-------

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


Git
~~~

.. code-block:: bash

   # Windows file permissions are wrong when using WSL
   git config --global core.filemode false

   # Do not use windows newline
   git config --global core.autocrlf true


Linux
-----

Compile libaries with UE4 clang
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   EngineDir="$SCRIPT_DIR/../../../"
   InstallClangDir="$EngineDir/Extras/ThirdPartyNotUE/SDKs/HostLinux/Linux_x64/v19_clang-11.0.1-centos7/x86_64-unknown-linux-gnu/"
   PATH="${InstallClangDir}/bin:$PATH"
   LibCxx="$EngineDir/Source/ThirdParty/Linux/LibCxx/lib/Linux/x86_64-unknown-linux-gnu/libc++.a"
   LibCxxAbi="$EngineDir/Source/ThirdParty/Linux/LibCxx/lib/Linux/x86_64-unknown-linux-gnu/libc++abi.a"
   LibCxxInclude="$EngineDir/Source/ThirdParty/Linux/LibCxx/include/c++/v1"

   cmake $ModuleDir/Source/ $Options \
      -DCMAKE_SYSROOT=${InstallClangDir}\
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_C_COMPILER=${InstallClangDir}/bin/clang \
      -DCMAKE_CXX_COMPILER=${InstallClangDir}/bin/clang++ \
      -DCMAKE_AR=${InstallClangDir}/bin/llvm-ar \
      -DCMAKE_NM=${InstallClangDir}/bin/llvm-nm \
      -DCMAKE_RANLIB=${InstallClangDir}/bin/x86_64-unknown-linux-gnu-ranlib \
      -DCMAKE_EXE_LINKER_FLAGS="-nostdinc++ -nostdlib++ -L$LibCxx --sysroot=${InstallClangDir} -fuse-ld=lld" \
      -DCMAKE_C_FLAGS="--sysroot=${InstallClangDir}" \
      -DCMAKE_CXX_FLAGS="-nostdlib++ --sysroot=${InstallClangDir} -isystem $LibCxxInclude"

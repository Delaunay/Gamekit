Cross compilation
=================


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

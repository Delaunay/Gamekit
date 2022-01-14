Source Compilation
==================


* You will need upward of 130 Go of space to compile UE4 with debug symbols


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

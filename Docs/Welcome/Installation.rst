Installation
============

`Releases`_

.. _`Releases`: https://gitlab.com/Delaunay/gamekit/-/releases


Source Code
------------

.. code-block:: bash

   # Go to your project directory
   cd /path/to/my/uproject

   # Add Gamekit as a submodule
   git submodule add https://github.com/Delaunay/Gamekit.git Plugins/Gamekit


Binary Installation Procedure
-----------------------------

1. Unzip inside `<Project>/Plugins`

Project structure after unzipping

.. code-block:: none

    <Project>/
    ├── Config/
    │   └── DefaultEditor.ini
    ├── Plugins/
    |   └── Gamekit
    |       ├── Source
    |       └── Gamekit.uplugin
    ├── Shaders
    ├── Source
    └── <Project>.uproject


2. Make the project load the plugin by modifying `<Project>.uproject`

.. code-block:: javascript

	"Modules": [
		{
			"Name": "<Project>",
			"Type": "Runtime",
			"LoadingPhase": "Default",
			"AdditionalDependencies": [
				"Engine",
				"AIModule"
			]
		},
		{
			"Name": "Gamekit",
			"Type": "Runtime",
			"LoadingPhase": "PostConfigInit",
			"AdditionalDependencies": [
				"Engine",
				"AIModule",
				"CoreUObject",
				"UMG",
				"GameplayAbilities",
				"OnlineSubsystem",
				"OnlineSubsystemUtils"
			]
		}
	],


3. Enable your C++ project to use Gamekit `Source/<Project>/<Project>.Build.cs`

.. code-block:: csharp

		PublicDependencyModuleNames.AddRange(new string[] {
                        ...
			"Gamekit"
		});


4. Configure your project


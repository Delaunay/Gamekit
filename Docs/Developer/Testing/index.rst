Testing
=======

* Ground Truth Data in the Functional Testing Editor plugin.
* Unit testing of C++ code
* Test Frameworks inside UE
  * `AutomationEditorCommon <https://github.com/EpicGames/UnrealEngine/tree/release/Engine/Source/Editor/UnrealEd/Public/Tests/AutomationEditorCommon.h>`_
    * Add ``UnrealEd`` Module
  * `AI Test Suite <https://github.com/EpicGames/UnrealEngine/tree/release/Engine/Source/Developer/AITestSuite>`_
  * `InputTestFramework <https://github.com/EpicGames/UnrealEngine/tree/release/Engine/Plugins/Experimental/EnhancedInput/Source/InputEditor/Private/Tests/InputTestFramework.h>`_

  https://github.com/EpicGames/UnrealEngine/tree/release/Engine/Plugins/Tests


Libraries
---------

* Google test is included
   * UnrealEngine/Engine/Source/ThirdParty/GoogleTest/GoogleTest.Build.cs
   * but nobody is using it ?
   * add GoogleTest
      * ``PublicDefinitions.Add("WITH_GOOGLE_MOCK=1");``
		* ``PublicDefinitions.Add("WITH_GOOGLE_TEST=1");``

* Fakeit is also included
   * UnrealEngine/Engine/Source/ThirdParty/FakeIt/FakeIt.Build.cs
   * but nobody is using it ?
   * https://github.com/eranpeer/FakeIt
   * Add FakeIt
      * PublicDefinitions.Add("WITH_FAKEIT=1");

* Catch2
   * UnrealEngine/Engine/Source/ThirdParty/Catch2
   * but nobody is using it ?

.. note::

   This `commit <https://github.com/EpicGames/UnrealEngine/commit/0d479b96bab63455be3f00d2680949191828d02a>`_
   suggest the libraries are used internally, but their tests are not
   included inside the engine's code.


Automation Tests
----------------


Example
~~~~~~~

.. code-block::

   // Gamekit Tests
   #include "GamekitTestSuite/GKTestUtilities.h"

   GAMEKIT_TEST("TestSubnames1.TestSubnames2", RandomClass, RandomMethod)
   {
      TestEqual("Check that stuff match", RandomMethod(X, Y), 123);

      return true;
   }


Get started
~~~~~~~~~~~

.. note::

   To get started I would suggest checking out `InputTestFramework` which seems to most publicly tested
   module out there.
   In addition it will teach you to mock user inputs to test your game bindings.


.. code-block::

   UWorld* AnEmptyWorld()
   {
      return FAutomationEditorCommonUtils::CreateNewMap();
   }

   UControllablePlayer& AControllablePlayer(UWorld* World)
   {
      EKeys::Initialize();

      UControllablePlayer* PlayerData = NewObject<UControllablePlayer>(World);

      PlayerData->Player = NewObject<APlayerController>(World->GetCurrentLevel());
      PlayerData->Player->InputComponent = NewObject<UEnhancedInputComponent>(PlayerData->Player);
      PlayerData->Player->PlayerInput = NewObject<UEnhancedPlayerInput>(PlayerData->Player);
      PlayerData->PlayerInput = Cast<UEnhancedPlayerInput>(PlayerData->Player->PlayerInput);
      PlayerData->InputComponent = Cast<UEnhancedInputComponent>(PlayerData->Player->InputComponent);
      PlayerData->Player->InitInputSystem();

      PlayerData->Subsystem.Reset(new FMockedEnhancedInputSubsystem(*PlayerData));

      check(PlayerData->IsValid());

      return *PlayerData;
   }

Mocking
~~~~~~~



Python Automation Test
----------------------

UE expects your python automation test to be saved inside ``Project/Content/Python``.
Files should be named ``test_*.py``. The tests will showup under the Editor section.

.. note::

   Plugins Content folder are not checked; but you can make a symbolic link to the plugin folder.

   .. code-block::

      C:/Users/Archimedes/work/gamekit/Content>mklink /D Python "../Plugins/Gamekit/Content/Python"
      symbolic link created for Python <<===>> ../Plugins/Gamekit/Content/Python

.. note::

   See C:/opt/UnrealEngine/Engine/Plugins/Experimental/PythonScriptPlugin/Content/Python/debugpy_unreal.py

* Examples (none of them showup in the UAT though)
   * C:/opt/UnrealEngine/Engine/Plugins/Enterprise/VariantManager/Extras/PythonAPI/test_variant_manager.py
   * C:/opt/UnrealEngine/Engine/Plugins/Experimental/PythonScriptPlugin/Content/Python/test_wrapper_types.py

* References for Unreal & python

   * https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/ScriptingAndAutomation/Python/
   * https://docs.unrealengine.com/5.0/en-US/PythonAPI/
   * https://docs.unrealengine.com/5.0/en-US/PythonAPI/module/unreal.html
   * https://docs.unrealengine.com/5.0/en-US/PythonAPI/class/EditorLoadingAndSavingUtils.html#unreal.EditorLoadingAndSavingUtils.new_map_from_template
   * https://docs.unrealengine.com/5.0/en-US/PythonAPI/class/EditorLoadingAndSavingUtils.html#unreal.EditorLoadingAndSavingUtils.new_map_from_template
   * https://docs.unrealengine.com/5.0/en-US/PythonAPI/class/AutomationScreenshotOptions.html#unreal.AutomationScreenshotOptions.ignore_colors
   * https://docs.unrealengine.com/5.0/en-US/PythonAPI/class/AutomationUtilsBlueprintLibrary.html#unreal.AutomationUtilsBlueprintLibrary
   * https://docs.unrealengine.com/5.0/en-US/PythonAPI/class/AutomationLibrary.html#unreal.AutomationLibrary

Example
~~~~~~~

Both printing and logging shows as

.. code-block:: python

   import unreal

   import automation_test.unittest_utilities as util

   run = util.TestRunner()

   @run.add_test
   def test_something():
      unreal.log("logging")
      print("printing")
      util.expect_false(lambda: False)


   @run.add_test
   @util.test_on_linux
   def test_something_linux():
      pass


   run.run_all()


Output

.. code-block::

   LogPython: Skipping test test_something_linux, current platform not supported.
   LogPython: Testing test_something
   LogPython: logging
   LogPython: printing


on failure the stack trace is printed

.. code-block::

   LogPython: Error: test_something - Exception: TestFailedException('Expected call to <lambda> to return False. (Actual: True)')
   LogPython: Error: Traceback (most recent call last):
   LogPython: Error:   File "C:/opt/UnrealEngine/Engine/Plugins/Tests/PythonAutomationTest/Content/Python/automation_test/unittest_utilities.py", line 125, in run_one_test
   LogPython: Error:     function_under_test()
   LogPython: Error:   File "C:/Users/Archimedes/work/gamekit/Content/Python/test_something.py", line 13, in test_something
   LogPython: Error:     util.expect_false(lambda: True)
   LogPython: Error:   File "C:/opt/UnrealEngine/Engine/Plugins/Tests/PythonAutomationTest/Content/Python/automation_test/unittest_utilities.py", line 374, in expect_false
   LogPython: Error:     return Expectation(expression, args).to_return(False)
   LogPython: Error:   File "C:/opt/UnrealEngine/Engine/Plugins/Tests/PythonAutomationTest/Content/Python/automation_test/unittest_utilities.py", line 223, in to_return
   LogPython: Error:     return self.to_return_internal(lambda result: result == expected_result, lambda result: '%s. (Actual: %s)' % (expected_result, result))
   LogPython: Error:   File "C:/opt/UnrealEngine/Engine/Plugins/Tests/PythonAutomationTest/Content/Python/automation_test/unittest_utilities.py", line 194, in to_return_internal
   LogPython: Error:     raise TestFailedException(message)
   LogPython: Error: automation_test.unittest_utilities.TestFailedException: Expected call to <lambda> to return False. (Actual: True)
   LogAutomationController: Error: Test Completed. Result={Failed} Name={test_something} Path={Editor.Python.test_something}
   LogAutomationController: BeginEvents: Editor.Python.test_something
   LogAutomationController: Error: test_something - Exception: TestFailedException('Expected call to <lambda> to return False. (Actual: True)') [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error: Traceback (most recent call last): [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:   File "C:/opt/UnrealEngine/Engine/Plugins/Tests/PythonAutomationTest/Content/Python/automation_test/unittest_utilities.py", line 125, in run_one_test [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:     function_under_test() [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:   File "C:/Users/Archimedes/work/gamekit/Content/Python/test_something.py", line 13, in test_something [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:     util.expect_false(lambda: True) [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:   File "C:/opt/UnrealEngine/Engine/Plugins/Tests/PythonAutomationTest/Content/Python/automation_test/unittest_utilities.py", line 374, in expect_false [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:     return Expectation(expression, args).to_return(False) [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:   File "C:/opt/UnrealEngine/Engine/Plugins/Tests/PythonAutomationTest/Content/Python/automation_test/unittest_utilities.py", line 223, in to_return [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:     return self.to_return_internal(lambda result: result == expected_result, lambda result: '%s. (Actual: %s)' % (expected_result, result)) [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:   File "C:/opt/UnrealEngine/Engine/Plugins/Tests/PythonAutomationTest/Content/Python/automation_test/unittest_utilities.py", line 194, in to_return_internal [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error:     raise TestFailedException(message) [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]
   LogAutomationController: Error: automation_test.unittest_utilities.TestFailedException: Expected call to <lambda> to return False. (Actual: True) [C:/opt/UnrealEngine/Engine/Source/Runtime/Core/Private/Logging/LogMacros.cpp(97)]


Functional Testing Actor
------------------------

* Add a ``AFunctionalTest`` actor in your level and call it fromt the level blueprint


Screen Comparison
-----------------

* ``Functional Screenshot Test Actor``
* Call ``Take Automation Screenshot at Camera`` inside blueprints
* Need to save ground truth to source control

Coverage
--------

Windows
~~~~~~~

Linux
~~~~~

* dll
* add -coverage flags


References
----------

.. [1] `Automation System Overview <https://docs.unrealengine.com/4.27/en-US/TestingAndOptimization/Automation/>`_
.. [2] `Automation Spec <https://docs.unrealengine.com/4.27/en-US/TestingAndOptimization/Automation/AutomationSpec/>`_

Scripting
=========


Call a UFUNCTION


.. code-block::


    const FGeneratedWrappedFunction& InFuncDef;
    UFunction* Func = InFuncDef.Func;

    FStructOnScope FuncParams(Func);

    // Calls the ufunction with its parameters allocated inside the struct memory
    InObj->ProcessEvent((UFunction*)InFunc, FuncParams.GetStructMemory());

    // return value is inside FuncParams
    ReturnValue = (const FProperty* Prop, FuncParams.GetStructMemory())



Lua
~~~

Overview
--------


.. note::

   You need to launch the editor once for the plugin to generate the Lua wrapeprs

* ScriptEditorPlugin

   * "Compiles" Lua to bytecode (for cooking), the compilation part does not seem to be implemented
      * Code is loaded in `FLuaContext::Initialize` when `luaL_loadstring(LuaState, TCHAR_TO_ANSI(*Code)` is called
      * We can use `lua_dump` to retrive the compiled bytecode
      * bytecode can be loaded using `lua_loadstring` NOT `luaL_loadstring` which is currently used

   * Specialize the Blueprint Editor

* ScriptGeneratorPlugin

   * Generates lua wrappers
   * Hooks into the UHT (Unreal Header Tools) to generate the wrappers

* ScriptPlugin

   * Manage the lua VM
   * Register new libraries at runtime
   * Call Lua Functions
   * All objects that interacts with LUA get a lua_state

      * Lua can be hooked to Tick, Destoy, BeginPlay

   * You can use this in lua to access the current object being implemented by the script

   * Objects that use lua create a ScriptBlueprint which holds the lua script
     so lua is next to the blueprint system. Used to be able to specify a source file.
     seems deprecated which might mean the code is bundled in the .asset instead of being
     readable text.



1. Create a new lua file inside the your content folder
2. When the editor is going to import the script it will ask you which
   class this script is inheriting from.
3. When the content of the script is modified the script will be reimported by the editor

Expose your own classes
-----------------------


Allow Lua Source file
---------------------


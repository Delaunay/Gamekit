// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGamekitEd, Log, All);

class FGamekitEdModule: public IModuleInterface
{
    public:
    virtual void StartupModule() override;

    virtual void ShutdownModule() override;

    // Helper to create too windows
    static void TriggerTool(UClass *ToolClass);
    static void CreateToolListMenu(class FMenuBuilder &MenuBuilder);
    static void OnToolWindowClosed(const TSharedRef<SWindow> &Window, class UGKEditorToolBase *Instance);


    // Add settings in the project windows
    void RegisterSettings();

    // add commands to our list
    void MakeCommandList();

    // Insert our new menus and toolbars
    void AddMenu();
    void AddToolBar();

    TSharedPtr<FUICommandList> CommandList;
};

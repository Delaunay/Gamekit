// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

// Include
#include "GamekitEd/GamekitEd.h"

// GamekitEd
#include "GamekitEd/Tools/GKEditorToolBase.h"
#include "GamekitEd/GamekitEdStyle.h"
#include "GamekitEd/GKCommands.h"
#include "GamekitEd/Tools/GKGameplayAbilityEditorTool.h"
#include "GamekitEd/Customization/GKAbilityStaticCustomization.h"

// Gamekit
#include "Gamekit/Abilities/GKAbilitySystemGlobals.h"
#include "Gamekit/GKGamekitSettings.h"

// Unreal Engine
#include "Engine/Blueprint.h"
#include "LevelEditor.h"
#include "Editor/DataTableEditor/Public/DataTableEditorModule.h"
#include "Modules/ModuleManager.h"
#include "ISettingsContainer.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"


DEFINE_LOG_CATEGORY(LogGamekitEd)

#define LOCTEXT_NAMESPACE "GamekitEdModule"

struct Local
{
    static void AddToolbarCommands(FToolBarBuilder &ToolbarBuilder)
    {
        ToolbarBuilder.AddToolBarButton(FGKCommands::Get().OpenPluginWindow);
    }

    static void AddMenuCommands(FMenuBuilder &MenuBuilder)
    {
        MenuBuilder.AddSubMenu(
            LOCTEXT("Gamekit", "Gamekit"),
            LOCTEXT("GamekitToolTip", "Gamekit Options"),
            FNewMenuDelegate::CreateStatic(&FGamekitEdModule::CreateToolListMenu)
        );
    }
};

void FGamekitEdModule::TriggerTool(UClass *ToolClass)
{
    UGKEditorToolBase *ToolInstance = NewObject<UGKEditorToolBase>(GetTransientPackage(), ToolClass);
    ToolInstance->AddToRoot();

    FPropertyEditorModule &PropPlugin = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    TArray<UObject *> ObjectsToView;
    ObjectsToView.Add(ToolInstance);

    TSharedRef<SWindow> Window = PropPlugin.CreateFloatingDetailsView(ObjectsToView, false);
    Window->SetOnWindowClosed(FOnWindowClosed::CreateStatic(&FGamekitEdModule::OnToolWindowClosed, ToolInstance));
}

void FGamekitEdModule::CreateToolListMenu(class FMenuBuilder &MenuBuilder)
{
    for (TObjectIterator<UClass> ClassIt; ClassIt; ++ClassIt)
    {
        UClass *Class = *ClassIt;

        if (Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_Abstract))
        {
            continue;
        }

        if (Class->IsChildOf(UGKEditorToolBase::StaticClass()))
        {
            FString FriendlyName = Class->GetName();

            // This get generate along side the blueprint stuff
            // but it is not executable
            if (!FriendlyName.StartsWith("SKEL_"))
            {
                FText   MenuDescription =
                        FText::Format(LOCTEXT("ToolMenuDescription", "{0}"), FText::FromString(FriendlyName));
                FText MenuTooltip =
                        FText::Format(LOCTEXT("ToolMenuToolTip", "Execute the {0} tool"), FText::FromString(FriendlyName));

                FUIAction Action(FExecuteAction::CreateStatic(&FGamekitEdModule::TriggerTool, Class));

                MenuBuilder.AddMenuEntry(MenuDescription, MenuTooltip, FSlateIcon(), Action);
            }
        }
    }
}

void FGamekitEdModule::OnToolWindowClosed(const TSharedRef<SWindow> &Window, UGKEditorToolBase *Instance)
{
    Instance->RemoveFromRoot();
}

void FGamekitEdModule::StartupModule()
{
    if (!FModuleManager::Get().IsModuleLoaded("LevelEditor"))
    {
        UE_LOG(LogGamekitEd, Error, TEXT("Could not load LevelEditor module, make sure the loading phase is correct"));
        return;
    }

    // Register customization for our tool widget
    {
        FPropertyEditorModule& PropPlugin = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

        PropPlugin.RegisterCustomClassLayout(
            "GKEditorToolBase",
            FOnGetDetailCustomizationInstance::CreateStatic(FGKBaseEditorToolCustomization::MakeInstance));
    }


    // Register Customization when editing the properties of AbilityStatic
    {
        // DataTableEditor does use the PropertyEditor with `CreateStructureDetailView`
        FPropertyEditorModule& PropPlugin = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

        PropPlugin.RegisterCustomClassLayout(
            "GKAbilityStatic",
            FOnGetDetailCustomizationInstance::CreateStatic(FGKAbilityStaticCustomization::MakeInstance));
    }

    RegisterSettings();

    // Load resources (icons etc...)
    FGamekitEdStyle::Initialize();

    // Register our commands so we can start calling them
    FGKCommands::Register();

    AddMenu();

    AddToolBar();

    EnableAutomaticDataRegeneration();
}

void FGamekitEdModule::MakeCommandList() {
    CommandList = MakeShareable(new FUICommandList);

    CommandList->MapAction(
        FGKCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateLambda([]() {
            // 
        }),
        FCanExecuteAction()
    );
}

void FGamekitEdModule::ExtendDataTableMenu() {
    FDataTableEditorModule& DataTableEditorModule = FModuleManager::LoadModuleChecked<FDataTableEditorModule>("DataTableEditor");

    // Add Menu
    TSharedRef<FExtender> MenuExtender(new FExtender());
    /*
    MenuExtender->AddMenuExtension("EditMain", // Existing section where we want to add it
        EExtensionHook::After,
        CommandList,
        FMenuExtensionDelegate::CreateStatic(&Local::AddMenuCommands));
    */

    DataTableEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
}

void FGamekitEdModule::AddToolBar() {
    // I think this one does not work for UE5 anymore
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    // Add Tool bar
    TSharedRef<FExtender> ToolBarExtender(new FExtender());
    ToolBarExtender->AddToolBarExtension("Content", // Existing section where we want to add it
        EExtensionHook::After,
        CommandList,
        FToolBarExtensionDelegate::CreateStatic(&Local::AddToolbarCommands));
    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(ToolBarExtender);
}


void FGamekitEdModule::AddMenu() {
    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

    // Add Menu
    TSharedRef<FExtender> MenuExtender(new FExtender());
    MenuExtender->AddMenuExtension("EditMain", // Existing section where we want to add it
        EExtensionHook::After,
        CommandList,
        FMenuExtensionDelegate::CreateStatic(&Local::AddMenuCommands));

    LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
    
}

void FGamekitEdModule::RegisterSettings() {
    ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));

    if (SettingsModule) {
        // Create the new category
        ISettingsContainerPtr SettingsContainer = SettingsModule->GetContainer(TEXT("Project"));

        SettingsContainer->DescribeCategory(
            TEXT("Gamekit Settings"),
            LOCTEXT("RuntimeWDCategoryName", "Gamekit Settings"),
            LOCTEXT("RuntimeWDCategoryDescription", "Gamekit Settings")
        );

        UGKGamekitSettings* Settings = UGKGamekitSettings::Get();

        // Register the settings
        ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings(
            TEXT("Project"), 
            TEXT("Plugins"), 
            TEXT("Gamekit"),
            LOCTEXT("RuntimeGeneralSettingsName", "Gamekit"),
            LOCTEXT("RuntimeGeneralSettingsDescription", "Gamekit"),
            Settings
        );

        // Register the save handler to your settings, you might want to use it to
        // validate those or just act to settings changes.
        if (SettingsSection.IsValid()) {
            SettingsSection->OnModified().BindLambda([]() {
                UGKGamekitSettings::Get()->SaveConfig();
                return true;
            });
        }
    }
}
void FGamekitEdModule::Dummy(FName Name, class UDataTable* Table) {
    UGKGameplayAbilityEditorTool::GenerateGameplayAbilitiesFromTable(Name, Table);
}

void FGamekitEdModule::EnableAutomaticDataRegeneration() {
    UGKGamekitSettings* Settings = UGKGamekitSettings::Get();

    if (Settings->bAutoRegenerateAbilities) {
        AbilityTableUpdate = Settings->GetOnAbilityTableChanged().AddStatic(&UGKGameplayAbilityEditorTool::GenerateGameplayAbilitiesFromTable);
        UE_LOG(LogGamekitEd, Log, TEXT("Auto Regenerating abilities %d"), Settings->GetOnAbilityTableChanged().IsBound());
    }

    if (Settings->bAutoRegenerateUnits) {
        UnitTableUpdate = Settings->GetOnUnitTableChanged().AddStatic(&UGKGameplayAbilityEditorTool::GenerateUnitsFromTable);
    }
}

void FGamekitEdModule::ShutdownModule() {
    if (UObjectInitialized()) {
        ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));

        if (SettingsModule) {
            SettingsModule->UnregisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("Gamekit"));
        }
    }

    FGamekitEdStyle::Shutdown();
    FGKCommands::Unregister();
}

IMPLEMENT_GAME_MODULE(FGamekitEdModule, GamekitEd);


#undef LOCTEXT_NAMESPACE
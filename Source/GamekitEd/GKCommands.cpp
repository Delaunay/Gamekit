

#include "GamekitEd/GKCommands.h"
#include "GamekitEd/GamekitEdStyle.h"

#define LOCTEXT_NAMESPACE "GamekitEd"

FGKCommands::FGKCommands() : TCommands<FGKCommands>(
    TEXT("GamekitEd"),
    NSLOCTEXT("Contexts", "GamekitEd", "GamekitEd Plugin"),
    NAME_None,
    FGamekitEdStyle::GetStyleSetName()) {}

void FGKCommands::RegisterCommands() {
    UI_COMMAND(OpenPluginWindow, "GamekitEd", "Open the GamekitEd window", EUserInterfaceActionType::Button, FInputChord{});
}

#undef LOCTEXT_NAMESPACE

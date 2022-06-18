#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"


class FGKCommands : public TCommands<FGKCommands> {
public:
    FGKCommands();

    virtual void RegisterCommands() override;

public:
    TSharedPtr<FUICommandInfo> OpenPluginWindow;
};

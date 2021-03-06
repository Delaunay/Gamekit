#pragma once

// UnrealEngine
#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

class FGamekitEdStyle {
public:
    static void Initialize();
    static void Shutdown();

    /** reloads textures used by slate renderer */
    static void ReloadTextures();

    static const ISlateStyle& Get();

    static FName GetStyleSetName();
private:
    static TSharedRef< class FSlateStyleSet > Create();
    static TSharedPtr< class FSlateStyleSet > StyleInstance;
};
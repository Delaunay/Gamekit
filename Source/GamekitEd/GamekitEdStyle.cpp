// Include
#include "GamekitEd/GamekitEdStyle.h"

// Unreal Engine
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"

static FName StyleSetName(TEXT("GamekitEdStyle"));
TSharedPtr< FSlateStyleSet > FGamekitEdStyle::StyleInstance = NULL;

void FGamekitEdStyle::Initialize() {
    if (!StyleInstance.IsValid()) {
        StyleInstance = Create();
        FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
    }
}

void FGamekitEdStyle::Shutdown() {
    FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
    ensure(StyleInstance.IsUnique());
    StyleInstance.Reset();
}

FName FGamekitEdStyle::GetStyleSetName() {
    return StyleSetName;
}


TSharedRef<FSlateStyleSet> FGamekitEdStyle::Create() {
    TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet(StyleSetName));
    Style->SetContentRoot(IPluginManager::Get().FindPlugin(TEXT("Gamekit"))->GetBaseDir() / TEXT("Resources"));

    const FString IconName = TEXT("GamekitEd");
    const FString Path = Style->RootToContentDir(IconName, TEXT(".png"));

    Style->Set(
        TEXT("GamekitEd.OpenPluginWindow"), 
        new FSlateImageBrush(Path, FVector2D(128, 128))
    );
    return Style;
}

void FGamekitEdStyle::ReloadTextures() {
    if (FSlateApplication::IsInitialized()) {
        FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
    }
}

const ISlateStyle& FGamekitEdStyle::Get() {
    return *StyleInstance;
}

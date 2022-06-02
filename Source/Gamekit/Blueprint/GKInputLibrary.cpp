// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#include "Gamekit/Blueprint/GKInputLibrary.h"

// Unreal Engine
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetInputLibrary.h"


FInputActionValue UGKInputLibrary::MakeBooleanAction(bool value) { return FInputActionValue(value); }

FInputActionValue UGKInputLibrary::MakeAxis1DAction(float value) { return FInputActionValue(value); }

FInputActionValue UGKInputLibrary::MakeAxis2DAction(FVector2D value) { return FInputActionValue(value); }

FInputActionValue UGKInputLibrary::MakeAxis3DAction(FVector value) { return FInputActionValue(value); }

bool UGKInputLibrary::IsEdgePanning(FVector2D MouseXYViewport, FVector4 Margins)
{
    return !FBox2D(FVector2D(Margins.X, Margins.Y), FVector2D(1.f - Margins.Z, 1.f - Margins.W))
                    .IsInside(MouseXYViewport);
}

FVector2D UGKInputLibrary::EdgePanDirection(FVector2D Position)
{

    Position = Position.ClampAxes(0, 1);

    // Invert XY axis to match expectation
    Position = FVector2D(Position.X, 1.f - Position.Y);

    // [0, 1] -> [0, 2] -> [-1, 1]
    Position = (Position * 2.f - 1.f);

    // Scale by a small factor for the truncation
    Position *= 1.05f;

    // Truncate to nearest Integer
    return FVector2D(FMath::TruncToFloat(Position.X), FMath::TruncToFloat(Position.Y));
}

const TArray<FInputActionKeyMapping>& UGKInputLibrary::GetKeysForAction(APlayerController* Controller, const FName& ActionName)
{
    return Controller->PlayerInput->GetKeysForAction(ActionName);
}

FText UGKInputLibrary::GetKeysForInputID(APlayerController* Controller, const EGK_MOBA_AbilityInputID InputID) {
    UEnum* Enum = StaticEnum<EGK_MOBA_AbilityInputID>();
    return GetKeysFromInputEnum(Controller, Enum, int(InputID));
}

FText UGKInputLibrary::GetKeysFromInputEnum(APlayerController* Controller, UEnum* Enum, int Value) {
    FText Name = Enum->GetDisplayNameTextByValue(int64(Value));

    auto& Results = GetKeysForAction(Controller, FName(*Name.ToString()));

    if (Results.Num() <= 0) {
        return FText();
    }

    return UKismetInputLibrary::Key_GetDisplayName(Results[0].Key);
}

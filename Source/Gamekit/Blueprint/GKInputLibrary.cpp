// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Gamekit/Blueprint/GKInputLibrary.h"

#include "Kismet/KismetSystemLibrary.h"


FInputActionValue UGKInputLibrary::MakeBooleanAction(bool value) {
    return FInputActionValue(value);
}

FInputActionValue UGKInputLibrary::MakeAxis1DAction(float value) {
    return FInputActionValue(value);
}

FInputActionValue UGKInputLibrary::MakeAxis2DAction(FVector2D value) {
    return FInputActionValue(value);
}

FInputActionValue UGKInputLibrary::MakeAxis3DAction(FVector value) {
    return FInputActionValue(value);
}

bool UGKInputLibrary::IsEdgePanning(FVector2D MouseXYViewport, FVector4 Margins) {
    return !FBox2D(
        FVector2D(Margins.X, Margins.Y), 
        FVector2D(1.f - Margins.Z, 1.f - Margins.W)
    ).IsInside(MouseXYViewport);
}


FVector2D UGKInputLibrary::EdgePanDirection(FVector2D Position) {
    
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

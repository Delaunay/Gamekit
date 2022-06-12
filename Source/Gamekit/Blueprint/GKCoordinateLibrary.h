// BSD 3-Clause License Copyright (c) 2022, Pierre Delaunay All rights reserved.

#pragma once

// Unreal Engine
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

// Generated
#include "GKCoordinateLibrary.generated.h"

/**
 * Should match the shader equivalent
 */
UCLASS()
class GAMEKIT_API UGKCoordinateLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    public:
    //! Returns coordinate (0, 1)
    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector2D ToTextureCoordinate(FVector WorldLocation, FVector2D MapSize);

    //! Returns coordinate (0, PixelSize)
    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector2D ToScreenCoordinate(FVector WorldLocation, FVector2D MapSize, FVector2D TextureSize);

    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector2D ToTextureCoordinate2D(FVector2D WorldLocation, FVector2D MapSize);

    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector2D ToScreenCoordinate2D(FVector2D WorldLocation, FVector2D MapSize, FVector2D TextureSize);

    UFUNCTION(BlueprintPure, Category = "Coordinate")
        static FVector ToProlateSpheroidal(FVector p, double Radius = 1) {

        double a = Radius;
        double p1 = FMath::Sqrt(p.X * p.X + p.Y * p.Y + (p.Z + a) * (p.Z + a));
        double p2 = FMath::Sqrt(p.X * p.X + p.Y * p.Y + (p.Z - a) * (p.Z - a));

        double Sigma = (p1 + p1) / (2.0 * a);
        double Tau = (p1 - p2) / (2.0 * a);
        double Phi = FMath::Atan2(p.Y, p.X);
        return FVector(Sigma, Tau, Phi);
    }

    UFUNCTION(BlueprintPure, Category = "Coordinate")
     static FVector FromProlateSpheroidal(FVector Point, double Radius = 1) {

        double s = Point.X;
        double t = Point.Y;
        double p = Point.Z;
        
        double v = Radius * FMath::Sqrt((s * s - 1) * (1 - t * t));

        return FVector(v * FMath::Cos(p), v * FMath::Sin(p), Radius * s * t);
    }

    UFUNCTION(BlueprintPure, Category = "Coordinate")
     static FVector FromToroidal(FVector Point, double Radius = 1) {
        double s = Point.X;
        double t = Point.Y;
        double p = Point.Z;
        double a = Radius;

        double top = sinh(t);
        double div = cosh(t) - FMath::Cos(s);

        return FVector(a * top / div * FMath::Cos(p), a * top / div * FMath::Sin(p), a * FMath::Sin(s) / div);
    }

    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector FromBispherical(FVector Point, double Radius = 1) {
        double s = Point.X;
        double t = Point.Y;
        double p = Point.Z;
        double a = Radius;

        double top = sinh(t);
        double div = cosh(t) - FMath::Cos(s);

        return FVector(
            a * FMath::Cos(p) * FMath::Sin(s) / (div),
            a * FMath::Sin(p) * FMath::Sin(s) / (div),
            a * sinh(t) / (div)
        );
    }

    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector FromParabolicCylindrical(FVector Point) {
        double s = Point.X;
        double t = Point.Y;
        double p = Point.Z;
        return FVector(s * t, (t * t - s * s) / 2, p);
    }

    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector FromOblateSpheroidal(FVector Point, double Radius = 1) {
        double a = Radius;

        double m = Point.X;
        double v = Point.Y;
        double p = Point.Z;
        return FVector(
            a * cosh(m) * FMath::Cos(v) * FMath::Cos(p),
            a * cosh(m) * FMath::Cos(v) * FMath::Sin(p),
            a * sinh(m) * FMath::Sin(v)
        );
    }

    UFUNCTION(BlueprintPure, Category = "Coordinate")
    static FVector FromBipolarCylindrical(FVector Point, double Radius = 1) {
        double a = Radius;

        double s = Point.X;
        double t = Point.Y;
        double z = Point.Z;
        return FVector(
            a * sinh(t) / (cosh(t) - FMath::Cos(s)),
            a * FMath::Sin(t) / (cosh(t) - FMath::Cos(s)),
            z
        );
    }
};

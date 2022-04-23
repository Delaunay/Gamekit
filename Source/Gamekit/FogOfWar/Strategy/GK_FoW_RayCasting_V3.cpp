#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V3.h"

// Gamekit
#include "Gamekit/Blueprint/GKCoordinateLibrary.h"
#include "Gamekit/Blueprint/GKUtilityLibrary.h"
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/GKTypes.h"

// Unreal Engine
#include "Kismet/KismetSystemLibrary.h"

UGKRayCasting_Less::UGKRayCasting_Less() {}

void TrackCoverage(TArray<FVector2D> &Coverage,
                   float64            AngleMin,
                   float64            AngleMax,
                   bool              &HasCoverage,
                   FVector2D         &PreviousAngle);

void UGKRayCasting_Less::DrawObstructedLineOfSight(class AGKFogOfWarTeam *FactionFog, UGKFogOfWarComponent *c)
{
    Triangles.Reset(c->TraceCount + 1);
    Coverage.Reset();

    // Initialize Cache Data
    ComponentOwner = c->GetOwner();
    OwnerLocation  = ComponentOwner->GetActorLocation();
    OwnerForward   = ComponentOwner->GetActorForwardVector();
    TraceType      = UEngineTypes::ConvertToTraceType(FogOfWarVolume->FogOfWarCollisionChannel);
    ActorsToIgnore = {ComponentOwner};
    AlreadySighted.Reset();
    bHasPrevious = false;

    ActorsToIgnore = {c->GetOwner()};
    ActorsToIgnore.Append(FogOfWarVolume->ActorsToIgnore);

    ActorClassFilter = AActor::StaticClass();
    ObjectTypes.Reset();
    UGKFogOfWarLibrary::ConvertToObjectType(FogOfWarVolume->FogOfWarCollisionChannel, ObjectTypes);
    // ------------------------

    TArray<AActor *> OutActors;

    UKismetSystemLibrary::CapsuleOverlapActors(
            GetWorld(), OwnerLocation, c->Radius, 1.f, ObjectTypes, ActorClassFilter, ActorsToIgnore, OutActors);

    FVector2D PreviousAngleRange;
    auto      BaseYaw     = ComponentOwner->GetActorRotation().Yaw;
    bool      HasCoverage = false;

    // Disable making the angle relative to the forward vector
    if (c->FieldOfView >= 360)
    {
        BaseYaw      = 0;
        OwnerForward = FVector(1, 0, 0);
    }

    float64 Offset = 0;
    float64 Margin = FogOfWarVolume->Margin;

    OutActors.Sort(
            [Location = OwnerLocation](const AActor &a, const AActor &b) -> bool
            {
                return UGKUtilityLibrary::GetYaw(Location, a.GetActorLocation()) >
                       UGKUtilityLibrary::GetYaw(Location, b.GetActorLocation());
            });

    TArray<float> Angles;
    for (auto &OutActor: OutActors)
    {
        FVector MinPoint;
        FVector MaxPoint = FVector(0, 0, 0);
        FVector Point;
        FVector Origin = OutActor->GetActorLocation();

        UGKUtilityLibrary::GetVisibleBounds(OwnerLocation, OutActor, MinPoint, MaxPoint);

        float64 Angle;
        float64 Radius;

        MinPoint = MinPoint - Origin;
        FMath::CartesianToPolar(MinPoint.X, MinPoint.Y, Radius, Angle);
        FMath::PolarToCartesian(Radius + Margin, Angle, Point.X, Point.Y);

        DebugDrawPoint(Point + Origin);
        auto Angle1 = UGKUtilityLibrary::GetYaw(OwnerLocation, Point + Origin) - BaseYaw;

        FMath::PolarToCartesian(Radius - Margin, Angle, Point.X, Point.Y);
        auto Angle2 = UGKUtilityLibrary::GetYaw(OwnerLocation, Point + Origin) - BaseYaw;

        // Other Side
        // ----------

        MaxPoint = MaxPoint - Origin;
        FMath::CartesianToPolar(MaxPoint.X, MaxPoint.Y, Radius, Angle);
        FMath::PolarToCartesian(Radius - Margin, Angle, Point.X, Point.Y);

        DebugDrawPoint(Point + Origin);
        auto Angle3 = UGKUtilityLibrary::GetYaw(OwnerLocation, Point + Origin) - BaseYaw;

        FMath::PolarToCartesian(Radius + Margin, Angle, Point.X, Point.Y);
        auto Angle4 = UGKUtilityLibrary::GetYaw(OwnerLocation, Point + Origin) - BaseYaw;

        //*
        Angles.Add(Angle1);
        Angles.Add(Angle2);
        Angles.Add(Angle3);
        Angles.Add(Angle4);
        //*/

        // Generates Triangles for this object
        // -------------------
        // Generate3Triangles(FactionFog, c, FVector4(Angle1, Angle2, Angle3, Angle4));
        // TrackCoverage(Coverage, Angle1, Angle4, HasCoverage, PreviousAngleRange);
    }

    /*/ Do a full turn
    auto Start = c->GetOwner()->GetActorLocation();
    for (int i = 1; i < Coverage.Num(); i++)
    {
        auto Prev    = Coverage[i - 1];
        auto Current = Coverage[i];

        auto p1 = CastLineFromAngle(FactionFog, c, Prev.Y).End;
        auto p2 = CastLineFromAngle(FactionFog, c, Prev.X).End;

        GenerateTriangle(c, Start, p1, p2);
    }
    UE_LOG(LogGamekit, Warning, TEXT("Full Turn %d"), Coverage.Num());
    */

    // Fill or remove angles
    // Do not sort the order is correct
    // Angles.Sort();
    FillMissingAngles(c, Angles);
    CastLinesFromAngles(FactionFog, c, Angles);

    // Only for non-dedicated servers
    GenerateTriangles(c);
    DrawTriangles(c);
}

void TrackCoverage(TArray<FVector2D> &Coverage,
                   float              AngleMin,
                   float              AngleMax,
                   bool              &HasCoverage,
                   FVector2D         &PreviousAngle)
{
    auto AngleCoverage = FVector2D(AngleMin, AngleMax);
    Coverage.Add(AngleCoverage);
    return;

    if (HasCoverage)
    {
        if (AngleCoverage.X < PreviousAngle.Y)
        {
            Coverage[Coverage.Num() - 1].Y = AngleCoverage.Y;
        }
        else
        {
            Coverage.Add(AngleCoverage);
        }
    }
    else
    {
        Coverage.Add(AngleCoverage);
    }
    PreviousAngle = AngleCoverage;
    HasCoverage   = true;
}

void UGKRayCasting_Less::Generate3Triangles(class AGKFogOfWarTeam *FactionFog,
                                            UGKFogOfWarComponent  *c,
                                            FVector4 const        &Angles)
{
    auto p1 = CastLineFromAngle(FactionFog, c, Angles.X).End;
    auto p2 = CastLineFromAngle(FactionFog, c, Angles.Y).End;
    auto p3 = CastLineFromAngle(FactionFog, c, Angles.Z).End;
    auto p4 = CastLineFromAngle(FactionFog, c, Angles.W).End;

    auto Start = c->GetOwner()->GetActorLocation();

    GenerateTriangle(c, Start, p1, p2);
    GenerateTriangle(c, Start, p2, p3);
    GenerateTriangle(c, Start, p3, p4);
}

// When a lot of units are in the same area we should cut the number of angles/lines
void UGKRayCasting_Less::FillMissingAngles(UGKFogOfWarComponent *c, TArray<float> &Angles)
{
    float MaxStep = c->FieldOfView / float(c->TraceCount);
    float MinStep = MaxStep / 2.f;
    int   n       = c->TraceCount / 2;

    if (Angles.Num() == 0)
    {
        for (int i = -n; i <= n; i++)
        {
            Angles.Add(float(i) * MaxStep);
        }
        return;
    }

    float MaxAngle = float(n) * MaxStep;
    float MinAngle = float(-n) * MaxStep;

    float Temp = Angles[0];
    Angles.Add(Temp + 360.f);

    TArray<float> NewAngles;
    NewAngles.Reserve(Angles.Num());

    NewAngles.Add(Angles[0]);
    PreviousAngle = Angles[0];

    for (int i = 1; i < Angles.Num(); i++)
    {
        float Angle = Angles[i];
        float Range = Angle - PreviousAngle;

        if (Range > MaxStep)
        {
            int   Count   = Range / MaxStep;
            float NewStep = Range / float(Count);

            for (int j = 1; j <= Count; j++)
            {
                NewAngles.Add(PreviousAngle + float(j) * NewStep);
            }
        }

        // Skip this angle because it is too close to previous one
        if (Range < MinAngle)
        {
            continue;
        }

        PreviousAngle = Angle;
        NewAngles.Add(PreviousAngle);
    }
    Angles       = NewAngles;
    bHasPrevious = false;
}

void UGKRayCasting_Less::CastLinesFromAngles(class AGKFogOfWarTeam *FactionFog,
                                             UGKFogOfWarComponent  *c,
                                             TArray<float>         &Angles)
{
    Lines.Reset(c->TraceCount + 1);
    for (auto Angle: Angles)
    {
        Lines.Add(CastLineFromAngle(FactionFog, c, Angle));
    }
}

FGKLinePoints UGKRayCasting_Less::CastLineFromAngle(class AGKFogOfWarTeam *FactionFog,
                                                    UGKFogOfWarComponent  *c,
                                                    float                  Angle)
{
    // We need to use forward vector in case the FieldOfView is not 360
    // Rotating by 0 is actually not stable
    FVector dir = OwnerForward.RotateAngleAxis(Angle, FVector(0, 0, 1));

    // We have to ignore the Inner Radius for our triangles to be
    // perfectly completing each other
    FVector LineStart = OwnerLocation; // + dir * c->InnerRadius;
    FVector LineEnd   = OwnerLocation + dir * c->Radius;
    DebugDrawPoint(LineEnd);

    bool Hit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
                                                     OwnerLocation,
                                                     LineEnd,
                                                     TraceType,
                                                     false, // bTraceComplex
                                                     ActorsToIgnore,
                                                     FogOfWarVolume->DebugTrace(),
                                                     OutHit,
                                                     true, // Ignore Self
                                                     FLinearColor::Red,
                                                     FLinearColor::Green,
                                                     0);

    float ExtendedRadius = c->Radius;
    if (bHasPrevious)
    {
        ExtendedRadius = FMath::Sqrt(
                FMath::Square(c->Radius / FMath::Tan(FMath::DegreesToRadians(90.f - (PreviousAngle - Angle) / 2.f))) +
                FMath::Square(c->Radius));
    }

    bHasPrevious  = true;
    PreviousAngle = Angle;

    LineEnd = Hit ? OutHit.Location - dir : OwnerLocation + dir * ExtendedRadius;

    auto HitActor = OutHit.HitObjectHandle.FetchActor<AActor>();

    if (Hit && HitActor)
    {
        // this only works if the actors are blocking the light
        // because our rays stops on obstacle only
        AddVisibleActor(FactionFog, c, HitActor);
    }

    return FGKLinePoints{LineStart, LineEnd};
}

void UGKRayCasting_Less::GenerateTriangle(UGKFogOfWarComponent *c, FVector Start, FVector End1, FVector End2)
{
    auto TexStart = FogOfWarVolume->GetTextureCoordinate(Start);
    auto TexEnd1  = FogOfWarVolume->GetTextureCoordinate(End1);
    auto TexEnd2  = FogOfWarVolume->GetTextureCoordinate(End2);

    auto         TriangleSize = FVector2D(c->Radius, c->Radius) * 2.f;
    FCanvasUVTri Triangle;

    // Triangle.V0_Color = FLinearColor::White;
    Triangle.V0_Pos = TexStart;
    Triangle.V0_UV  = FVector2D(0.5, 0.5);

    // Triangle.V1_Color = FLinearColor::White;
    Triangle.V1_Pos = TexEnd1;
    Triangle.V1_UV  = UGKCoordinateLibrary::ToTextureCoordinate((End1 - Start), TriangleSize);

    // Triangle.V2_Color = FLinearColor::White;
    Triangle.V2_Pos = TexEnd2;
    Triangle.V2_UV  = UGKCoordinateLibrary::ToTextureCoordinate((End2 - Start), TriangleSize);

    Triangles.Add(Triangle);
}
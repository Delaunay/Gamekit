#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V3.h"

#include "FogOfWar/GKFogOfWarVolume.h"
#include "FogOfWar/GKFogOfWarComponent.h"
#include "FogOfWar/GKFogOfWarLibrary.h"

#include "Blueprint/GKCoordinateLibrary.h"
#include "Blueprint/GKUtilityLibrary.h"

#include "Kismet/KismetSystemLibrary.h"


#define DEBUG_CIRCLE(XY)                    \
    {if (FogOfWarVolume->bDebug){           \
    UKismetSystemLibrary::DrawDebugCircle(  \
        GetWorld(),                         \
        XY,                                 \
        25.f,                               \
        36,                                 \
        FLinearColor::White,                \
        0.f,                                \
        5.f,                                \
        FVector(1, 0, 0),                   \
        FVector(0, 1, 0),                   \
        true                                \
    );                                      \
    }}

UGKRayCasting_Less::UGKRayCasting_Less(){}

void UGKRayCasting_Less::DrawObstructedLineOfSight(UGKFogOfWarComponent *c)
{
    Triangles.Reset(c->TraceCount + 1);

    TArray<AActor *>                      ActorsToIgnore   = {c->GetOwner()};
    ActorsToIgnore.Append(FogOfWarVolume->ActorsToIgnore);

    UClass *                              ActorClassFilter = AActor::StaticClass();
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    UGKFogOfWarLibrary::ConvertToObjectType(FogOfWarVolume->FogOfWarCollisionChannel, ObjectTypes);

    AActor *         Actor = c->GetOwner();
    TArray<AActor *> OutActors;

    FVector Location = Actor->GetActorLocation();
    FVector Forward  = Actor->GetActorForwardVector();

    UKismetSystemLibrary::CapsuleOverlapActors(
            GetWorld(), Location, c->Radius, 1.f, ObjectTypes, ActorClassFilter, ActorsToIgnore, OutActors);

    // UE_LOG(LogGamekit, Log, TEXT("Found %d Actors"), OutActors.Num());

    TArray<FVector> EndPoints;
    TArray<float>   Angles;
    FVector         Origin;
    FVector         BoxExtent;
    FVector         Cartesian;
    auto            BaseYaw = Actor->GetActorRotation().Yaw;

    // Disable making the angle relative to the forward vector
    if (c->FieldOfView >= 360)
    {
        BaseYaw = 0;
    }

    float Offset   = 0;
    float Margin   = FogOfWarVolume->Margin;

    OutActors.Sort(
            [&Location](const AActor &a, const AActor &b) -> bool
            {
                return UGKUtilityLibrary::GetYaw(Location, a.GetActorLocation()) >
                       UGKUtilityLibrary::GetYaw(Location, b.GetActorLocation());
            });

    for (auto &OutActor: OutActors)
    {
        FVector MinPoint;
        FVector MaxPoint = FVector(0, 0, 0);
        FVector Point;
        UGKUtilityLibrary::GetVisibleBounds(Location, OutActor, MinPoint, MaxPoint);

        float Angle;
        float Radius;

        MinPoint = MinPoint - Origin;
        FMath::CartesianToPolar(MinPoint.X, MinPoint.Y, Radius, Angle);
        FMath::PolarToCartesian(Radius + Margin, Angle, Point.X, Point.Y);

        DEBUG_CIRCLE(Point + Origin);
        auto Angle1 = UGKUtilityLibrary::GetYaw(Location, Point + Origin) - BaseYaw;

        Angles.Add(Angle1);

        FMath::PolarToCartesian(Radius - Margin, Angle, Point.X, Point.Y);
        auto Angle2 = UGKUtilityLibrary::GetYaw(Location, Point + Origin) - BaseYaw;
        Angles.Add(Angle2);

        // Other Side
        // ----------

        MaxPoint = MaxPoint - Origin;
        FMath::CartesianToPolar(MaxPoint.X, MaxPoint.Y, Radius, Angle);

        FMath::PolarToCartesian(Radius - Margin, Angle, Point.X, Point.Y);
        DEBUG_CIRCLE(Point + Origin);
        auto Angle3 = UGKUtilityLibrary::GetYaw(Location, Point + Origin) - BaseYaw;
        Angles.Add(Angle3);

        FMath::PolarToCartesian(Radius + Margin, Angle, Point.X, Point.Y);
        auto Angle4 = UGKUtilityLibrary::GetYaw(Location, Point + Origin) - BaseYaw;
        Angles.Add(Angle4);
    }

    Angles.Sort();

    FillMissingAngles(c, Angles);

    GenerateTrianglesFromAngles(c, Angles);

    DrawTriangles(c);
} 

void UGKRayCasting_Less::FillMissingAngles(UGKFogOfWarComponent *c, TArray<float> &Angles)
{
    float MaxStep = c->FieldOfView / float(c->TraceCount);
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
    float PreviousAngle = Angles[0];

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
        PreviousAngle = Angle;
        NewAngles.Add(PreviousAngle);
    }
    Angles = NewAngles;
}

void UGKRayCasting_Less::GenerateTrianglesFromAngles(UGKFogOfWarComponent *c, TArray<float>& Angles) {
    Triangles.Reset(c->TraceCount + 1);

    auto Actor       = c->GetOwner();
    FVector Location = Actor->GetActorLocation();
    FVector Forward  = Actor->GetActorForwardVector();

     // Disable making the angle relative to the forward vector
    if (c->FieldOfView >= 360)
    {
        Forward = FVector(1, 0, 0);
    }

    TArray<AActor *> ActorsToIgnore   = {Actor};

    FCanvasUVTri   Triangle;
    FHitResult     OutHit;
    auto           TraceType = UEngineTypes::ConvertToTraceType(FogOfWarVolume->FogOfWarCollisionChannel);
    FVector2D      Previous = FVector2D::ZeroVector;
    FVector        LinePrevious = FVector::ZeroVector;
    float          PreviousAngle = 0;
    bool           bHasPrevious = false;
    auto           TriangleSize = FVector2D(c->Radius, c->Radius) * 2.f;
    TSet<AActor *> AlreadySighted;

    for (auto Angle: Angles)
    {
        // Not sure why we get a bunch of angles that are very close
        /*
        if (bHasPrevious && FMath::IsNearlyEqual(Angle, PreviousAngle))
        {
            continue;
        }
        //*/

        // We need to use forward vector in case the FieldOfView is not 360
        // Rotating by 0 is actually not stable
        FVector dir = Forward.RotateAngleAxis(Angle, FVector(0, 0, 1));

        // We have to ignore the Inner Radius for our triangles to be
        // perfectly completing each other
        FVector LineStart = Location; // + dir * c->InnerRadius;
        FVector LineEnd   = Location + dir * c->Radius;

        if (FogOfWarVolume->bDebug)
        {
            UKismetSystemLibrary::DrawDebugCircle(
                GetWorld(),          // World
                LineEnd,             // Center
                25.f,                // Radius
                36,                  // NumSegments
                FLinearColor::White, // LineColor
                0.f,                 // LifeTime 
                5.f,                 // Tickness
                FVector(1, 0, 0),    // YAxis 
                FVector(0, 1, 0),    // ZAxis 
                true                 // DrawAxes
            );
        }

        bool hit = UKismetSystemLibrary::LineTraceSingle(
            GetWorld(),
            Location,
            LineEnd,
            TraceType,
            false, // bTraceComplex
            ActorsToIgnore,
            FogOfWarVolume->DebugTrace(),
            OutHit,
            true, // Ignore Self
            FLinearColor::Red,
            FLinearColor::Green,
            5.0f
        );

        // TODO: compute the extended radius here
        float ExtendedRadius = bHasPrevious ? FMath::Sqrt(
            FMath::Square(c->Radius / FMath::Tan(FMath::DegreesToRadians(90.f - (PreviousAngle - Angle) / 2.f)))
            + FMath::Square(c->Radius)
        ): c->Radius;

        LineEnd = hit ? OutHit.Location - dir : Location + dir * ExtendedRadius;

        auto Start = FogOfWarVolume->GetTextureCoordinate(LineStart);
        auto End   = FogOfWarVolume->GetTextureCoordinate(LineEnd);

        //Triangle.V0_Color = FLinearColor::White;
        Triangle.V0_Pos   = Start;
        Triangle.V0_UV  = FVector2D(0.5, 0.5);

        //Triangle.V1_Color = FLinearColor::White;
        Triangle.V1_Pos   = Previous;
        Triangle.V1_UV    = UGKCoordinateLibrary::ToTextureCoordinate((LinePrevious - LineStart), TriangleSize);

        //Triangle.V2_Color = FLinearColor::White;
        Triangle.V2_Pos   = End;
        Triangle.V2_UV    = UGKCoordinateLibrary::ToTextureCoordinate((LineEnd - LineStart), TriangleSize);

        float step = c->FieldOfView / float(c->TraceCount);
        if (bHasPrevious)
        {
            /*
            UE_LOG(LogGamekit,
                   Log,
                   TEXT("Triangle V1 %s -> V2 %s  A1 %f -> A2 %f"),
                   *Triangle.V1_Pos.ToString(),
                   *Triangle.V2_Pos.ToString(),
                   PreviousAngle,
                   Angle);
            //*/

            if (FMath::Abs(Angle - PreviousAngle) > 180)
            {
                UE_LOG(LogGamekit, Log, TEXT("Angle too big skipping"));
            }
            else
            {
                Triangles.Add(Triangle);
            }
        }

        bHasPrevious = true;
        Previous = End;
        LinePrevious = LineEnd;
        PreviousAngle = Angle;
        
        if (hit && OutHit.Actor.IsValid())
        {
            // Avoid multiple broadcast per target
            AActor *Target = OutHit.Actor.Get();
            if (!AlreadySighted.Contains(Target))
            {
                AlreadySighted.Add(Target);
                BroadCastEvents(Actor, c, Target);
            }
        }
    }
}
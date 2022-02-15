#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V3.h"

#include "FogOfWar/GKFogOfWarVolume.h"
#include "FogOfWar/GKFogOfWarComponent.h"
#include "FogOfWar/GKFogOfWarLibrary.h"

#include "Blueprint/GKCoordinateLibrary.h"
#include "Blueprint/GKUtilityLibrary.h"

#include "Kismet/KismetSystemLibrary.h"


UGKRayCasting_Less::UGKRayCasting_Less(){}

void UGKRayCasting_Less::DrawObstructedLineOfSight(UGKFogOfWarComponent *c) {
    Triangles.Reset(c->TraceCount + 1);

    TArray<AActor *>                      ActorsToIgnore   = {
        c->GetOwner()
    };
    UClass *                              ActorClassFilter = AActor::StaticClass();
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    UGKFogOfWarLibrary::ConvertToObjectType(FogOfWarVolume->FogOfWarCollisionChannel, ObjectTypes);

    AActor *                              Actor            = c->GetOwner();
    TArray<AActor *>                      OutActors;

    FVector Location = Actor->GetActorLocation();
    FVector Forward  = Actor->GetActorForwardVector();

    /*
    OverlapMultiByObjectType(
            TArray< struct FOverlapResult > & OutOverlaps,
            const FVector & Pos,
            const FQuat & Rot,
            const FCollisionObjectQueryParams & ObjectQueryParams,
            const FCollisionShape & CollisionShape,
            const FCollisionQueryParams & Params
        ) const;

        */
    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(), 
        Location, 
        c->Radius, 
        ObjectTypes, 
        ActorClassFilter,
        ActorsToIgnore, 
        OutActors
    );

    // UE_LOG(LogGamekit, Log, TEXT("Found %d Actors"), OutActors.Num());

    TArray<FVector> EndPoints;
    TArray<float> Angles;
    FVector Origin;
    FVector BoxExtent;
    FVector Cartesian;
    auto BaseYaw = Actor->GetActorRotation().Yaw;
    float step = c->FieldOfView / float(c->TraceCount);
    int   n    = c->TraceCount / 2;

    float MaxAngle = float(n) * step;
    float MinAngle = float(-n) * step;
    float Offset   = 0;
    float Margin   = FogOfWarVolume->Margin;

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
        auto Angle3 = UGKUtilityLibrary::GetYaw(Location, Point + Origin) - BaseYaw;
        Angles.Add(Angle3);

        FMath::PolarToCartesian(Radius + Margin, Angle, Point.X, Point.Y);
        auto Angle4 = UGKUtilityLibrary::GetYaw(Location, Point + Origin) - BaseYaw;
        Angles.Add(Angle4);
    }

    //*
    for (int i = -n; i <= n; i++)
    {
        float angle = float(i) * step;
        Angles.Add(angle);
    }
    //*/

    Angles.Sort();

    /*/ make sure the last triangle do a full turn
    if (c->FieldOfView >= 360)
    {
        // TArray does not want to add its own element
        float Temp = Angles[0];
        Angles.Add(Temp);
    } */

    /*
    // Make sure the angles are not too far appart
    float step = c->FieldOfView / float(c->TraceCount);

    Angles.Sort();
    TArray<float> NewAngles;
    float         Previous = Angles[0];
    NewAngles.Add(Previous);
    Angles.Add(Previous);

    for (int i = 1; i < Angles.Num(); i++)
    {
        auto Angle = Angles[i];
        auto Range = (Angle - Previous);
        auto Diff = int(Range / step);

        for (int j = 1; j < Diff; j++)
        {
            NewAngles.Add(Previous + Range / float(Diff) * j);
        }

        NewAngles.Add(Angle);
    }
    //*/

    GenerateTrianglesFromAngles(c, Angles);

    DrawTriangles(c);
}

void UGKRayCasting_Less::GenerateTrianglesFromAngles(UGKFogOfWarComponent *c, TArray<float>& Angles) {
    Triangles.Reset(c->TraceCount + 1);

    auto Actor       = c->GetOwner();
    FVector Location = Actor->GetActorLocation();
    FVector Forward  = Actor->GetActorForwardVector();

    TArray<AActor *> ActorsToIgnore   = {};

    FCanvasUVTri   Triangle;
    FHitResult     OutHit;
    auto           TraceType = UEngineTypes::ConvertToTraceType(FogOfWarVolume->FogOfWarCollisionChannel);
    FVector2D      Previous;
    FVector        LinePrevious;
    float          PreviousAngle;
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

        LineEnd = hit ? OutHit.Location : Location + dir * ExtendedRadius;

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

        if (bHasPrevious)
        {
            Triangles.Add(Triangle);
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
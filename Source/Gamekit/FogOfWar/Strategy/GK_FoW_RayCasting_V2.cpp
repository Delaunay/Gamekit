#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V2.h"

#include "FogOfWar/GKFogOfWarVolume.h"
#include "FogOfWar/GKFogOfWarComponent.h"
#include "Blueprint/GKCoordinateLibrary.h"

#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UGKRayCasting_Triangle::UGKRayCasting_Triangle() {}

void UGKRayCasting_Triangle::DrawObstructedLineOfSight(UGKFogOfWarComponent *c) {
    Triangles.Reset(c->TraceCount + 1);

    AActor *         actor          = c->GetOwner();
    FVector          forward        = actor->GetActorForwardVector();
    FVector          loc            = actor->GetActorLocation();
    TArray<AActor *> ActorsToIgnore = {actor};
    TSet<AActor *>   AlreadySighted;

     // Disable making the angle relative to the forward vector
    if (c->FieldOfView >= 360)
    {
        forward = FVector(1, 0, 0);
    }

    float step = c->FieldOfView / float(c->TraceCount);
    int   n    = c->TraceCount / 2;

    FHitResult   OutHit;
    FCanvasUVTri Triangle;
    auto         TraceType = UEngineTypes::ConvertToTraceType(FogOfWarVolume->FogOfWarCollisionChannel);
    FVector2D    Previous;
    FVector      LinePrevious;
    bool         bHasPrevious = false;

    // Because we are drawing triangles but our field of view is arched
    // we make the radius bigger the circle outline will be drawn using the texture
    float halfStep       = step / 2.f;
    float ExtendedRadius = FMath::Sqrt(FMath::Square(c->Radius / FMath::Tan(FMath::DegreesToRadians(90.f - halfStep))) +
                                       FMath::Square(c->Radius));

    auto TriangleSize = FVector2D(c->Radius, c->Radius) * 2.f;

    // UE_LOG(LogGamekit, Log, TEXT("Extended Radius is %f (%f)"), ExtendedRadius, c->Radius);

    for (int i = -n; i <= n; i++)
    {
        float   angle = float(i) * step;
        FVector dir   = forward.RotateAngleAxis(angle, FVector(0, 0, 1));

        // We have to ignore the Inner Radius for our triangles to be
        // perfectly completing each other
        FVector LineStart = loc; // + dir * c->InnerRadius;
        FVector LineEnd   = loc + dir * c->Radius;

        bool hit = UKismetSystemLibrary::LineTraceSingle(FogOfWarVolume->GetWorld(),
                                                         LineStart,
                                                         LineEnd,
                                                         TraceType,
                                                         false, // bTraceComplex
                                                         ActorsToIgnore,
                                                         FogOfWarVolume->DebugTrace(),
                                                         OutHit,
                                                         true, // Ignore Self
                                                         FLinearColor::Red,
                                                         FLinearColor::Green,
                                                         5.0f);

        LineEnd = hit ? OutHit.Location : loc + dir * ExtendedRadius;

        auto Start = FogOfWarVolume->GetTextureCoordinate(LineStart);

        // FIXME: Effective radius turns out to be a bit smaller
        auto End = FogOfWarVolume->GetTextureCoordinate(LineEnd);

        // Triangle.V0_Color = FLinearColor::White;
        Triangle.V0_Pos = Start;
        Triangle.V0_UV  = FVector2D(0.5, 0.5);

        // Triangle.V1_Color = FLinearColor::White;
        Triangle.V1_Pos = Previous;
        Triangle.V1_UV  = UGKCoordinateLibrary::ToTextureCoordinate((LinePrevious - LineStart), TriangleSize);

        // Triangle.V2_Color = FLinearColor::White;
        Triangle.V2_Pos = End;
        Triangle.V2_UV  = UGKCoordinateLibrary::ToTextureCoordinate((LineEnd - LineStart), TriangleSize);

        if (bHasPrevious)
        {
            Triangles.Add(Triangle);
        }

        bHasPrevious = true;
        Previous     = End;
        LinePrevious = LineEnd;

        if (hit && OutHit.Actor.IsValid())
        {
            // Avoid multiple broadcast per target
            AActor *Target = OutHit.Actor.Get();
            if (!AlreadySighted.Contains(Target))
            {
                AlreadySighted.Add(Target);
                BroadCastEvents(actor, c, Target);
            }
        }
    }

    DrawTriangles(c);
}

void UGKRayCasting_Triangle::DrawTriangles(UGKFogOfWarComponent *c)
{
    // Draw all Triangles
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    UCanvasRenderTarget2D* RenderCanvas = GetFactionRenderTarget(c->Faction, true);
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
        FogOfWarVolume->GetWorld(), 
        RenderCanvas, 
        Canvas, 
        Size, 
        Context
    );

    Canvas->K2_DrawMaterialTriangle(FogOfWarVolume->TrianglesMaterial, Triangles);

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
    Triangles.Reset(c->TraceCount + 1);
} 

#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V2.h"

// Gamekit
#include "Gamekit/Blueprint/GKCoordinateLibrary.h"
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"

// Unreal Engine
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UGKRayCasting_Triangle::UGKRayCasting_Triangle() {}

void UGKRayCasting_Triangle::DrawObstructedLineOfSight(class AGKTeamFog *FactionFog, UGKFogOfWarComponent *c)
{
    AActor *         actor          = c->GetOwner();
    FVector          forward        = actor->GetActorForwardVector();
    FVector          loc            = actor->GetActorLocation();
    TArray<AActor *> ActorsToIgnore = {actor};
    ActorsToIgnore.Append(FogOfWarVolume->ActorsToIgnore);

    TSet<AActor *> AlreadySighted;

    // Disable making the angle relative to the forward vector
    if (c->FieldOfView >= 360)
    {
        forward = FVector(1, 0, 0);
    }

    float step = c->FieldOfView / float(c->TraceCount);
    int   n    = c->TraceCount / 2;

    FHitResult OutHit;
    auto       TraceType = UEngineTypes::ConvertToTraceType(FogOfWarVolume->FogOfWarCollisionChannel);

    // Because we are drawing triangles but our field of view is arched
    // we make the radius bigger the circle outline will be drawn using the texture
    float halfStep       = step / 2.f;
    float ExtendedRadius = FMath::Sqrt(FMath::Square(c->Radius / FMath::Tan(FMath::DegreesToRadians(90.f - halfStep))) +
                                       FMath::Square(c->Radius));

    Lines.Reset(c->TraceCount + 1);
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
                                                         0);

        // Increase the radius slightly
        LineEnd = hit ? OutHit.Location : loc + dir * ExtendedRadius;
        Lines.Add(FGKLinePoints{LineStart, LineEnd});

        auto HitActor = OutHit.HitObjectHandle.FetchActor<AActor>();
        if (hit && HitActor)
        {
            // this only works if the actors are blocking the light
            // because our rays stops on obstacle only
            AddVisibleActor(FactionFog, c, HitActor);
        }
    }

    GenerateTriangles(c);
    DrawTriangles(c);
}

void UGKRayCasting_Triangle::GenerateTriangles(UGKFogOfWarComponent *c)
{
    // Rendering is not needed by the server
#if !UE_SERVER
    if (GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    FCanvasUVTri Triangle;
    FVector2D    Previous;
    FVector      LinePrevious;
    bool         bHasPrevious = false;
    auto         TriangleSize = FVector2D(c->Radius, c->Radius) * 2.f;

    Triangles.Reset(c->TraceCount + 1);

    for (auto Line: Lines)
    {
        auto Start = FogOfWarVolume->GetTextureCoordinate(Line.Start);
        auto End   = FogOfWarVolume->GetTextureCoordinate(Line.End);

        // Triangle.V0_Color = FLinearColor::White;
        Triangle.V0_Pos = Start;
        Triangle.V0_UV  = FVector2D(0.5, 0.5);

        // Triangle.V1_Color = FLinearColor::White;
        Triangle.V1_Pos = Previous;
        Triangle.V1_UV  = UGKCoordinateLibrary::ToTextureCoordinate((LinePrevious - Line.Start), TriangleSize);

        // Triangle.V2_Color = FLinearColor::White;
        Triangle.V2_Pos = End;
        Triangle.V2_UV  = UGKCoordinateLibrary::ToTextureCoordinate((Line.End - Line.Start), TriangleSize);

        if (bHasPrevious)
        {
            Triangles.Add(Triangle);
        }

        bHasPrevious = true;
        Previous     = End;
        LinePrevious = Line.End;
    }
#endif
}

void UGKRayCasting_Triangle::DrawTriangles(UGKFogOfWarComponent *c)
{
    // Rendering is not needed by the server
#if !UE_SERVER
    if (GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    // Draw all Triangles
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    UCanvasRenderTarget2D *RenderCanvas = GetFactionRenderTarget(c->GetFaction(), true);

    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(
            FogOfWarVolume->GetWorld(), RenderCanvas, Canvas, Size, Context);

    Canvas->K2_DrawMaterialTriangle(FogOfWarVolume->TrianglesMaterial, Triangles);

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
#endif
}

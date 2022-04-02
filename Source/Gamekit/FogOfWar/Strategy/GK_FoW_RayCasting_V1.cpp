
#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V1.h"

// Gamekit
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/FogOfWar/GKFogOfWarActorTeam.h"


// Unreal Engine
#include "Engine/Canvas.h"
#include "Engine/CanvasRenderTarget2D.h"
#include "Engine/TextureRenderTarget2DArray.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Rendering/Texture2DResource.h"

UGKRayCasting_Line::UGKRayCasting_Line() {}

void UGKRayCasting_Line::Initialize()
{
    Super::Initialize();

    auto TexScale = FogOfWarVolume->TextureScale;
    auto MapSize  = FogOfWarVolume->MapSize;

    if (TexScale == 0)
    {
        UE_LOG(LogGamekit, Warning, TEXT("TextureScale cannot be zero"));

        // If size == 0 it will trigger assert on the RHI side
        TexScale = 1.f;
    }

    FogOfWarVolume->SetTextureSize(FVector2D(MapSize.X, MapSize.Y) * TexScale);
}

void UGKRayCasting_Line::DrawFactionFog(class AGKFogOfWarActorTeam *FactionFog)
{
    auto Texture = GetFactionRenderTarget(FactionFog->Name);

    FactionFog->Vision = Texture;
    UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), Texture);

    for (auto &Component: FactionFog->Allies)
    {
        DrawLineOfSight(FactionFog, Component);
    }
}

void UGKRayCasting_Line::DrawLineOfSight(class AGKFogOfWarActorTeam *FactionFog, UGKFogOfWarComponent *c)
{
    if (!c->GivesVision)
    {
        return;
    }

    DebugDrawComponent(c);

    if (c->UnobstructedVision)
    {
        DrawUnobstructedLineOfSight(FactionFog, c);
    }
    else
    {
        DrawObstructedLineOfSight(FactionFog, c);
    } 
}

void UGKRayCasting_Line::DrawObstructedLineOfSight(class AGKFogOfWarActorTeam *FactionFog, class UGKFogOfWarComponent *c)
{
    AActor *         actor          = c->GetOwner();
    FVector          forward        = actor->GetActorForwardVector();
    FVector          loc            = actor->GetActorLocation();
    TArray<AActor *> ActorsToIgnore = {GetOwner()};
    ActorsToIgnore.Append(FogOfWarVolume->ActorsToIgnore);

    float      step = c->FieldOfView / float(c->TraceCount);
    int        n    = c->TraceCount / 2;
    FHitResult OutHit;
    auto       TraceType = UEngineTypes::ConvertToTraceType(FogOfWarVolume->FogOfWarCollisionChannel);

    Lines.Reset(c->TraceCount + 1);

    for (int i = -n; i <= n; i++)
    {
        float   angle = float(i) * step;
        FVector dir   = forward.RotateAngleAxis(angle, FVector(0, 0, 1));

        FVector LineStart = loc + dir * c->InnerRadius;
        FVector LineEnd   = loc + dir * c->Radius;

        bool hit = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
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

        LineEnd = hit ? OutHit.Location : LineEnd;
        Lines.Add(FGKLinePoints{LineStart, LineEnd});

        if (hit && OutHit.Actor.IsValid())
        {
            AddVisibleActor(FactionFog, c, OutHit.Actor.Get());
        }
    }

    DrawLines(c);
}

void UGKRayCasting_Line::DrawLines(class UGKFogOfWarComponent *c)
{
    // Rendering is not needed by the server
#if !UE_SERVER
    if (GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    // Draw lines
    auto RenderCanvas = GetFactionRenderTarget(c->GetFaction(), true);
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderCanvas, Canvas, Size, Context);
    for (auto Line: Lines)
    {
        auto Start = FogOfWarVolume->GetTextureCoordinate(Line.Start);
        auto End   = FogOfWarVolume->GetTextureCoordinate(Line.End);

        Canvas->K2_DrawLine(Start, End, c->LineTickness, FLinearColor::White);
    }

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);

#endif
}

UTexture *UGKRayCasting_Line::GetFactionTexture(FName name, bool bCreateRenderTarget)
{
    return GetFactionRenderTarget(name, bCreateRenderTarget);
}

//!
UCanvasRenderTarget2D *UGKRayCasting_Line::GetFactionRenderTarget(FName Name, bool bCreateRenderTarget)
{
    UCanvasRenderTarget2D **RenderResult = FogFactions.Find(Name);
    UCanvasRenderTarget2D * Render       = nullptr;

    if (RenderResult != nullptr)
    {
        Render = RenderResult[0];
    }
    else if (bCreateRenderTarget && !IsBeingDestroyed())
    {
        Render = CreateRenderTarget();
        FogFactions.Add(Name, Render);
    }
    return Render;
}

UCanvasRenderTarget2D *UGKRayCasting_Line::CreateRenderTarget()
{
    FogOfWarVolume->GetBrushSizes(FogOfWarVolume->TextureSize, FogOfWarVolume->MapSize);

    UE_LOG(LogGamekit,
           Log,
           TEXT("Creating a new render target (%.2f x %.2f)"),
           FogOfWarVolume->TextureSize.X,
           FogOfWarVolume->TextureSize.Y);

    auto Texture = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(GetWorld(),
                                                                     UCanvasRenderTarget2D::StaticClass(),
                                                                     FogOfWarVolume->TextureSize.X,
                                                                     FogOfWarVolume->TextureSize.Y);

    /*
    Texture->InitCustomFormat(
        FogOfWarVolume->TextureSize.X,
        FogOfWarVolume->TextureSize.Y,
        EPixelFormat::PF_G16, // G8 is not supported
        false
    );
    */
    return Texture;
}

void UGKRayCasting_Line::DrawUnobstructedLineOfSight_Draw(class AGKFogOfWarActorTeam *FactionFog, UGKFogOfWarComponent *c)
{
#if !UE_SERVER
    if (GetWorld()->GetNetMode() == NM_DedicatedServer)
    {
        return;
    }

    AActor *actor   = c->GetOwner();
    FVector forward = actor->GetActorForwardVector();

    UMaterialInstanceDynamic *material = UKismetMaterialLibrary::CreateDynamicMaterialInstance(
            GetWorld(), FogOfWarVolume->UnobstructedVisionMaterial, NAME_None, EMIDCreationFlags::None);

    FLinearColor Value;
    Value.R = forward.X;
    Value.G = forward.Y;
    Value.B = c->FieldOfView;
    Value.A = c->InnerRadius / c->Radius;
    material->SetVectorParameterValue("Direction&FoV", Value);

    auto RenderCanvas = GetFactionRenderTarget(c->GetFaction(), true);
    auto NewRadius    = FVector2D(c->Radius * FogOfWarVolume->TextureSize.X / FogOfWarVolume->MapSize.X,
                               c->Radius * FogOfWarVolume->TextureSize.Y / FogOfWarVolume->MapSize.Y);
    auto Start        = FogOfWarVolume->GetTextureCoordinate(actor->GetActorLocation()) - NewRadius;

    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderCanvas, Canvas, Size, Context);

    Canvas->K2_DrawMaterial(Cast<UMaterialInterface>(material),
                            Start,
                            NewRadius * 2.f,
                            FVector2D(0.f, 0.f),
                            FVector2D::UnitVector,
                            0.f,
                            FVector2D(0.5f, 0.5f));

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
#endif
}

void UGKRayCasting_Line::DrawUnobstructedLineOfSight(class AGKFogOfWarActorTeam *FactionFog, UGKFogOfWarComponent *c)
{
    DrawUnobstructedLineOfSight_Draw(FactionFog, c);

    AActor *         Actor = c->GetOwner();
    TArray<AActor *> ActorsToIgnore;
    ActorsToIgnore.Append(FogOfWarVolume->ActorsToIgnore);
    UClass *                              ActorClassFilter = AActor::StaticClass();
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    UGKFogOfWarLibrary::ConvertToObjectType(FogOfWarVolume->FogOfWarCollisionChannel, ObjectTypes);
    TArray<AActor *> OutActors;

    UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(), Actor->GetActorLocation(), c->Radius, ObjectTypes, ActorClassFilter, ActorsToIgnore, OutActors);

    for (AActor *Target: OutActors)
    {
        if (Target == nullptr)
        {
            continue;
        }
        AddVisibleActor(FactionFog, c, Target);
    }
}
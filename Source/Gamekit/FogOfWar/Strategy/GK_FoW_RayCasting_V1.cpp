
#include "Gamekit/FogOfWar/Strategy/GK_FoW_RayCasting_V1.h"

#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"
#include "Gamekit/FogOfWar/GKFogOfWarComponent.h"
#include "Gamekit/FogOfWar/GKFogOfWarLibrary.h"
#include "Gamekit/FogOfWar/GKFogOfWarVolume.h"

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

void UGKRayCasting_Line::DrawFactionFog(FGKFactionFog *FactionFog)
{
    auto Texture       = GetFactionRenderTarget(FactionFog->Name);

    FactionFog->Vision = Texture;
    UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), Texture);

    for (auto &Component: FactionFog->Allies)
    {
        DrawLineOfSight(Component);
    }
}

void BroadCastEvents(AActor *Seer, UGKFogOfWarComponent *SeerComponent, AActor *Target)
{
    // Send an event that current actor is seeing something
    SeerComponent->OnSighting.Broadcast(Target);

    UActorComponent *component = Target->GetComponentByClass(UGKFogOfWarComponent::StaticClass());
    if (component == nullptr)
    {
        return;
    }

    UGKFogOfWarComponent *target = Cast<UGKFogOfWarComponent>(component);
    if (target == nullptr)
    {
        return;
    }

    // Send an event that the hit is being seen
    target->OnSighted.Broadcast(Seer);
}

void UGKRayCasting_Line::DrawLineOfSight(UGKFogOfWarComponent *c)
{
    if (!c->GivesVision)
    {
        return;
    }

    DebugDrawComponent(c);

    if (c->UnobstructedVision)
    {
        DrawUnobstructedLineOfSight(c);
    }
    else
    {
        DrawObstructedLineOfSight(c);
    }
}

void UGKRayCasting_Line::DrawObstructedLineOfSight(class UGKFogOfWarComponent *c)
{
    AActor *         actor          = c->GetOwner();
    FVector          forward        = actor->GetActorForwardVector();
    FVector          loc            = actor->GetActorLocation();
    TArray<AActor *> ActorsToIgnore = {GetOwner()};
    TSet<AActor *>   AlreadySighted;

    float step = c->FieldOfView / float(c->TraceCount);
    int   n    = c->TraceCount / 2;

    FHitResult                 OutHit;
    UCanvas *                  Canvas;
    FVector2D                  Size;
    FDrawToRenderTargetContext Context;

    auto RenderCanvas = GetFactionRenderTarget(c->GetFaction(), true);
    UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderCanvas, Canvas, Size, Context);
    auto TraceType = UEngineTypes::ConvertToTraceType(FogOfWarVolume->FogOfWarCollisionChannel);

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
                                                         5.0f);

        LineEnd = hit ? OutHit.Location : LineEnd;

        auto Start = FogOfWarVolume->GetTextureCoordinate(LineStart);
        auto End   = FogOfWarVolume->GetTextureCoordinate(LineEnd);

        //
        //
        Canvas->K2_DrawLine(Start, End, c->LineTickness, FLinearColor::White);

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

    UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
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
    else if (bCreateRenderTarget)
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

    auto Texture = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
        GetWorld(),
        UCanvasRenderTarget2D::StaticClass(),
        FogOfWarVolume->TextureSize.X,
        FogOfWarVolume->TextureSize.Y
    );

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

void UGKRayCasting_Line::DrawUnobstructedLineOfSight(UGKFogOfWarComponent *c)
{
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

    TArray<AActor *>                      ActorsToIgnore;
    UClass *                              ActorClassFilter = AActor::StaticClass();
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    UGKFogOfWarLibrary::ConvertToObjectType(FogOfWarVolume->FogOfWarCollisionChannel, ObjectTypes);
    TArray<AActor *> OutActors;

    UKismetSystemLibrary::SphereOverlapActors(
            GetWorld(), actor->GetActorLocation(), c->Radius, ObjectTypes, ActorClassFilter, ActorsToIgnore, OutActors);

    for (AActor *Target: OutActors)
    {
        if (Target == nullptr)
        {
            continue;
        }

        BroadCastEvents(actor, c, Target);
    }
}
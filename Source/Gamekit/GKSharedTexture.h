// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GKSharedTexture.generated.h"

/**
 * Used to share Game Play data from the Game to the shader
 * It works by drawing pixel on a texture, the texture is then read by the shader and can be interpreted
 * as needed
 *
 * 1. In the editor create a new Canvas Render Target
 * 2. Create a new Blueprint inheriting this class
 * 3. Set your Blueprint.RenderTarget to use the Render target you created
 *
 * Call AddPoint/RemovePoint to insert new data points onto the texture
 * Your render target can be used in a material to fetch GamePlay information
 *
 * Notes
 * -----
 * When accessing/setting the data you should keep in mind that texture coordinate are
 * (0, 0) (top-left corner) (1, 1) (bottom-right corner)
 *
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAMEKIT_API UGKSharedTexture : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGKSharedTexture();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Texture, meta = (AllowPrivateAccess = "true"))
	class UCanvasRenderTarget2D* RenderTarget;

	UFUNCTION(BlueprintGetter)
    class UCanvasRenderTarget2D* GetRenderTarget() const { return RenderTarget; }

	UFUNCTION(BlueprintSetter)
    void SetRenderTarget(class UCanvasRenderTarget2D* v) { RenderTarget = v; }

	// (0, 0) is top-left corner
	UFUNCTION(BlueprintCallable)
	void AddPoint(FIntPoint Index, FLinearColor Value);

	//! Same as AddPoint but (0, 0) is in the middle of the texture
	UFUNCTION(BlueprintCallable)
	void AddPointCentered(FIntPoint Index, FLinearColor Value);

	UFUNCTION(BlueprintCallable)
	void RemovePoint(FIntPoint Index);

	UFUNCTION(BlueprintCallable)
	FColor ReadPoint(FIntPoint p);

	UFUNCTION(BlueprintCallable)
	FColor ReadCenteredPoint(FIntPoint p);

private:
	class UTexture2D* CPUTexture;
};

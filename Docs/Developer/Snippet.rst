


FMessageLog("Blueprint").Warning(LOCTEXT("RenderTargetSampleUV_InvalidRenderTarget", "RenderTargetSampleUVEditoOnly: Render Target must be non-null."));




DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(
    FActorBeginCursorOverSignature,     // SparseDelegateClass
    AActor,                             // OwningClass
    OnBeginCursorOver,                  // DelegateName
    AActor*,                            // Param1Type
    TouchedActor                        // Param1Name
);

UPROPERTY(BlueprintAssignable, Category = "Collision")
FWhateverDelegate OnWhatever;
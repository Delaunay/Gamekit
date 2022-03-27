// BSD 3-Clause License Copyright (c) 2019, Pierre Delaunay All rights reserved.


#include "Projectiles/GKAbilityEffectActor.h"

// Sets default values
AGKAbilityEffectActor::AGKAbilityEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AGKAbilityEffectActor::ApplyToTargets() {
	// Iterate list of gameplay effects
	for (const FGameplayEffectSpecHandle& SpecHandle : GameplayEffects.TargetGameplayEffectSpecs)
	{
		if (SpecHandle.IsValid())
		{
			// If effect is valid, iterate list of targets and apply to all
			for (TSharedPtr<FGameplayAbilityTargetData> Data : GameplayEffects.TargetData.Data)
			{
				Data->ApplyGameplayEffectSpec(*SpecHandle.Data.Get());
			}
		}
	}
}

void AGKAbilityEffectActor::AddTargetToEffectContainerSpec(AActor* TargetActor) {
	TArray<AActor*> TargetActors;
	TargetActors.Add(TargetActor);
	GameplayEffects.AddTargets(TArray<FHitResult>(), TargetActors);
}

void AGKAbilityEffectActor::AddHitResultToEffectContainerSpec(const FHitResult& HitResult) {
	TArray<FHitResult> HitResults;
	HitResults.Add(HitResult);
	GameplayEffects.AddTargets(HitResults, TArray<AActor*>());
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

protected:
	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

	UPROPERTY(EditAnywhere, Category = Config)
		class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = Config)
		class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, Category = Config)
		float Damage = 20.f;

private:
	UPROPERTY(EditAnywhere, Category = Config)
		UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere, Category = Config)
		UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, Category = Config)
		class USoundCue* FireSound;

	/*
	* Trace end with Scatter
	 */

	UPROPERTY(EditAnywhere, Category = Config = WeaponScatter)
		float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = Config = WeaponScatter)
		float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category = Config = WeaponScatter)
		bool bUseScatter = false;
};

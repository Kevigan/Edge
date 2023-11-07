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
	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

	UPROPERTY(EditAnywhere, Category = Config)
		class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = Config)
		class USoundCue* HitSound;

	

private:
	UPROPERTY(EditAnywhere, Category = Config)
		UParticleSystem* BeamParticles;

	UPROPERTY(EditAnywhere, Category = Config)
		UParticleSystem* MuzzleFlash;

	UPROPERTY(EditAnywhere, Category = Config)
		class USoundCue* FireSound;



	
};

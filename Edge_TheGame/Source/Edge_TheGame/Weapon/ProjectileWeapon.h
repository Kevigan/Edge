// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Edge_TheGame/Weapon/Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

private:
	UPROPERTY(EditAnywhere, Category = Config)
		TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = Config)
		TSubclassOf<AProjectile> ServerSideRewindProjectileClass;
};

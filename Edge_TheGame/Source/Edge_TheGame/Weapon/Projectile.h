// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class EDGE_THEGAME_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, Category = Config)
		float Damage = 20.f;
private:
	UPROPERTY(EditAnywhere)
		class UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY()
		class UParticleSystemComponent* TracerComponent;

	UPROPERTY(EditAnywhere, Category = Config)
		class UParticleSystem* Tracer;

	UPROPERTY(EditAnywhere, Category = Config)
		UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = Config)
		class USoundCue* ImpactSound;


public:

};

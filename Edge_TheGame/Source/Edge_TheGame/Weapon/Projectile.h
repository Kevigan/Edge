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

	/*
	*  Used with server-side rewind
	*/
	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere, Category = Config)
		float InitialSpeed = 15000.f;

	float Damage = 20.f;

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnHitBodyProjectile(FVector Location, FRotator Rotation);


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* CollisionBox = nullptr;
private:

	UPROPERTY()
		class UParticleSystemComponent* TracerComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = Config)
		class UParticleSystem* Tracer;

	UPROPERTY(EditAnywhere, Category = Config)
		UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere, Category = Config)
		class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere, Category = Config)
		USoundCue* ImpactSoundBody;

	USoundCue* ChosenImpactSound;


public:

};

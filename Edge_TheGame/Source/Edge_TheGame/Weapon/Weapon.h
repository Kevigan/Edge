// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped State"),
	EWS_Dropped UMETA(DisplayName = "Dropped State"),

	EWS_MAX UMETA(DisplayName = "DedaultMAX")
};

UCLASS()
class EDGE_THEGAME_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);

	/// <summary>
	/// Textures for the weapon crosshairs
	/// </summary>

	UPROPERTY(EditAnywhere, Category = Config)
		class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Config)
		class UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Config)
		class UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Config)
		class UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Config)
		class UTexture2D* CrosshairsBottom;

	/// <summary>
	/// Zoomed FOV while aiming
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Config = WeaponSettings)
		float ZoomedFOV = 30.f;

	UPROPERTY(EditAnywhere, Category = Config = WeaponSettings)
		float ZoomInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere, Category = Config = WeaponSettings)
		float CrosshairShootingFactor = 0.75f;

	/// <summary>
	/// Auto fire
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Config = WeaponSettings)
		float FireDelay = .5f;

	UPROPERTY(EditAnywhere, Category = Config = WeaponSettings)
		bool bAutomatic = false;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

	UFUNCTION()
		void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		);

private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
		EWeaponState WeaponState;

	UFUNCTION()
		void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = Config)
		class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = Config)
		TSubclassOf<class ACasing> CasingClass;



public:

	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Edge_TheGame/HUD/Edge_HUD.h"
#include "Edge_TheGame/Weapon/WeaponTypes.h"
#include "Edge_TheGame/EdgeTypes/CombatState.h"
#include "CombatComponent.generated.h"


class AWeapon;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EDGE_THEGAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();
	friend class AEdgeCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapons();
	void Reload();
	FLinearColor ColorToChange;

	void FireButtonPressed(bool bPressed);
	bool bLocallyReloading = false;
protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
		void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
		void OnRep_EquippedWeapon();

	UFUNCTION()
		void OnRep_SecondaryWeapon();

	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTarget);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastShptgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
		void ServerReload();

	void HandleReload();

	UFUNCTION(BlueprintCallable)
		void FinishReloading();

	UFUNCTION(BlueprintCallable)
		void FinishSwap();

	UFUNCTION(BlueprintCallable)
		void FinishSwapAttachWeapons();

	int32 AmountToReload();

	void DropEquippedWeapon();
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToBackpack(AActor* ActorToAttach);
	void UpdateCarriedAmmo();
	void PlayEquippedWeaponSound(AWeapon* WeaponToEquip);
	void ReloadEmptyWeapon();
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

	class UEdgeGameInstance* EdgeGameInstance;

	

	UFUNCTION(BlueprintCallable)
		void OnClientJoinMidGame();

	FString GetCurrentSkinWeaponType(EWeaponType CurrentWeaponType);

private:
	UPROPERTY()
		class AEdgeCharacter* Character = nullptr;
	UPROPERTY()
		class AEdgePlayerController* Controller = nullptr;
	UPROPERTY()
		class AEdge_HUD* HUD = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
		AWeapon* EquippedWeapon = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
		AWeapon* SecondaryWeapon = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
		bool bAiming = false;

	bool bAimButtonPressed = false;

	UFUNCTION()
		void OnRep_Aiming();

	UPROPERTY(EditAnywhere)
		float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
		float AimWalkSpeed;

	bool bFireButtonPressed;

	/// <summary>
	/// HUD and Crosshairs
	/// </summary>
	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;
	FHUDPackage HUDPackage;

	FVector HitTarget;

	/// <summary>
	/// Aiming and FOV
	/// </summary>

		//FOV when not aiming set to the cameras base FOV in BeginPlay
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
		float ZoomedFOV = 30.f;

	float CurrentFOV;

	UPROPERTY(EditAnywhere, Category = Combat)
		float ZoomInterpSpeed = 20.f;

	void InterpFOV(float DeltaTime);

	/// <summary>
	///  Automatic fire
	/// </summary>

	FTimerHandle FireTimer;

	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

	//Carried Ammo for the currently equipped weapon
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
		int32 CarriedAmmo;

	UFUNCTION()
		void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere, Category = Config = Ammo)
		int32 StartingARAmmo = 30;

	UPROPERTY(EditAnywhere, Category = Config = Ammo)
		int32 StartingPistolAmmo = 30;

	UPROPERTY(EditAnywhere, Category = Config = Ammo)
		int32 StartingShotgunAmmo = 30;

	UPROPERTY(EditAnywhere, Category = Config = Ammo)
		int32 StartingSniperAmmo = 15;
	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
		ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
		void OnRep_CombatState();

	void UpdateAmmoValues();

public:
	void SetCrossHairCOlor(FLinearColor color) { ColorToChange = color; }
	bool ShouldSwapWeapons();

	UFUNCTION(BlueprintCallable)
		AWeapon* GetEquippedWeapon() { return EquippedWeapon; }

	UFUNCTION(BlueprintCallable)
		AWeapon* GetSecondaryWeapon() { return SecondaryWeapon; }

};

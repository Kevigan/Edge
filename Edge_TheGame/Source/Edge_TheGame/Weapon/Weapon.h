// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped State"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped State"),

	EWS_MAX UMETA(DisplayName = "DedaultMAX")
};

UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName = "Hit Scan Weapon"),
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"),
	EFT_Shotgun UMETA(DisplayName = "Shotgun Weapon"),

	EFT_MAX UMETA(DisplayName = "DedaultMAX")
};

UENUM(BlueprintType)
enum class ESkinType : uint8
{
	EFT_Common UMETA(DisplayName = "Common"),
	EFT_UnCommon UMETA(DisplayName = "UnCommon"),
	EFT_Rare UMETA(DisplayName = "Rare"),
	EFT_Epic UMETA(DisplayName = "Epic"),
	EFT_Legendary UMETA(DisplayName = "Legendary"),
	EFT_Mythic UMETA(DisplayName = "Mythic"),

	EFT_MAX UMETA(DisplayName = "DedaultMAX")
};


UCLASS()
class EDGE_THEGAME_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void SetHUDAmmo();
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);
	void Dropped();
	void AddAmmo(int32 AmmoToAdd);
	FVector TraceEndWithScatter(const FVector& HitTarget);

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

	UPROPERTY(EditAnywhere, Category = Config = WeaponSettings)
		class USoundCue* EquipSound;

	bool bDestroyWeapon = false;

	UPROPERTY(EditAnywhere, Category = Config)
		EFireType FireType;

	UPROPERTY(EditAnywhere, Category = Config = WeaponScatter)
		bool bUseScatter = false;

	UPROPERTY(EditAnywhere, Category = Config = DrawDebug)
		bool bDrawDebug = false;

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnEquipped(const FString& SkinType);

	UFUNCTION(Server, Reliable)
		void Server_ChangeSkin(const FString& Skin);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_ChangeSKin(const FString& Skin);

	UFUNCTION()
		void Test(AEdgeCharacter* OwnerCHar);

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveTEST();

	FString GetCurrentSkinWeaponType();

	UPROPERTY(BlueprintReadWrite)
		FString CurrentSkinType;

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnSpawnWeapon();

	UFUNCTION(BlueprintCallable, Client, Reliable)
		void Client_SpawnWeapon();

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_SpawnWeapon();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SpawnWeapon();

protected:
	virtual void BeginPlay() override;
	virtual void OnWeaponStateSet();
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();

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

	/*
	* Trace end with Scatter
	*/

	UPROPERTY(EditAnywhere, Category = Config = WeaponScatter)
		float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category = Config = WeaponScatter)
		float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category = Config)
		float Damage = 20.f;


	UPROPERTY(Replicated, EditAnywhere, Category = Config)
		bool bUseServerSideRewind = false;

	UPROPERTY()
		class AEdgeCharacter* EdgeOwnerCharacter = nullptr;

	UPROPERTY()
		class AEdgePlayerController* EdgeOwnerController = nullptr;

	UFUNCTION()
		void OnPingTooHigh(bool bPingTooHigh);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Weapon Properties")
		USkeletalMeshComponent* WeaponMesh = nullptr;



private:

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class USphereComponent* AreaSphere = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
		EWeaponState WeaponState;

	UFUNCTION()
		void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class UWidgetComponent* PickupWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = Config)
		class UAnimationAsset* FireAnimation = nullptr;

	UPROPERTY(EditAnywhere, Category = Config)
		TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere)
		int32 Ammo;

	UFUNCTION(Client, Reliable)
		void ClientUpdateAmmo(int32 ServerAmmo);

	UFUNCTION(Client, Reliable)
		void ClientAddAmmo(int32 AmmoToAdd);

	void SpendRound();

	UPROPERTY(EditAnywhere, Category = Config)
		int32 MagCapacity;

	// The number of unprocessed server requests for Ammo
	// Incremented in SpendROund, decremented in ClientUpdateAmmo
	int32 Sequence = 0;

	UPROPERTY(EditAnywhere, Category = Config)
		EWeaponType WeaponType;

	class UEdgeGameInstance* EdgeGameInstance;

public:

	void SetWeaponState(EWeaponState State);
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	bool IsEmpty();
	bool IsFull();
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	FORCEINLINE float GetDamage() const { return Damage; }

	UFUNCTION(BlueprintCallable)
		EWeaponType GetWeaponTypeBP() { return WeaponType; }
};

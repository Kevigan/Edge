// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Edge_TheGame/EdgeTypes/TurningInPlace.h"
#include "Edge_TheGame/Interfaces/InteractWithCrosshairsInterface.h"
#include "Edge_TheGame/EdgeTypes/CombatState.h"
#include "EdgeCharacter.generated.h"

UCLASS()
class EDGE_THEGAME_API AEdgeCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	AEdgeCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	virtual void OnRep_ReplicatedMovement() override;
	virtual void Destroyed() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
		void MulticastElim();

	UPROPERTY()
		class AEdgePlayerState* EdgePlayerState = nullptr;

	UPROPERTY(Replicated)
		bool bDisableGameplay = false;

	//Change crosshair color on hit and on death
	void ChangeCrosshairColor(float EnemyHealth);

	UFUNCTION(Client, Reliable)
		void ClientChangeCrosshairColor(float EnemyHealth);

	FTimerHandle CrosshairTimer;
	void CrosshairTimerFinished();

	void AddKillText(AEdgeCharacter* EdgeCharacter);

	UFUNCTION(Client, Reliable)
		void ClientAddKillText(AEdgeCharacter* EdgeCharacter);

	UFUNCTION(BlueprintImplementableEvent)
		void ShowSniperScopeWidget(bool bShowScope);

	UFUNCTION(BlueprintImplementableEvent)
		void HideShowSniperScopeWidget();

	void SpawnDefaultWeapon();
protected:
	virtual void BeginPlay() override;

	void MoveFoward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void MouseWheelTurned();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void EscapeButtonPressed();
	void UpdateHUDHealth();
	void UpdateHUDAmmo();
	void PlayHitUI();
	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();

	UFUNCTION(Client, Reliable)
		void SpawnPlayerIndicator();

	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void ReceiveOnSpawnPlayerIndicator();

	void SpawnMiniMapOnBeginPlay();

	UFUNCTION(Server, Reliable)
		void SpawnMiniMapServer();

	UFUNCTION(Client, Reliable)
		void ClientSpawnMiniMap();

	void SpawnMiniMap();

	float ControllerYaw;

	FTimerHandle InitializeMiniMapTimer;
	void InitializeMiniMapTimerFinished();

	UPROPERTY(EditAnywhere, Category = Config)
		TSubclassOf<class AActor> MiniMapClass;

	AActor* MiniMapActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Config)
		TSubclassOf<class AActor> PlayerIndicatorClass;

	AActor* PlayerIndicatorActor = nullptr;

	UFUNCTION()
		void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	void RotateInPlace(float DeltaTime);
private:
	class AEdge_HUD* HUD;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* OverHeadWidget = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon = nullptr;

	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* Combat = nullptr;

	UFUNCTION(Server, Reliable)
		void ServerEquipButtonPressed();

	UFUNCTION(Server, Reliable)
		void ServerMouseWheelTurned();

	UFUNCTION(Server, Reliable)
		void ServerSpawnDefaultWeapon();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	/// <summary>
	/// Montages
	/// </summary>

	UPROPERTY(EditAnywhere, Category = Config = Combat)
		class UAnimMontage* FireWeaponMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = Config = Combat)
		class UAnimMontage* ReloadMontage = nullptr;

	UPROPERTY(EditAnywhere, Category = Config = Combat)
		class UAnimMontage* ElimMontage = nullptr;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
		float CameraThreshold = 200.f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationsLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	/// <summary>
	///  Player health
	/// </summary>
	UPROPERTY(EditAnywhere, Category = Config = PlayerStats)
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = Config = PlayerStats)
		float Health = 100.f;

	UFUNCTION()
		void OnRep_Health();

	UPROPERTY()
		class AEdgePlayerController* EdgePlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;
	void ElimTimerFinished();
	UPROPERTY(EditDefaultsOnly)
		float ElimDelay = 3.f;
	// Poll for any relevant classes and initialize our HUD
	void PollInit();

	UPROPERTY(VisibleAnywhere, Category = Config)
		class USpringArmComponent* SpringarmMiniMap;

	UPROPERTY(VisibleAnywhere, Category = Config)
		class USceneCaptureComponent2D* SceneCaptureMiniMap2D;

	/**
	 * Default Weapon
	 */
	UPROPERTY(EditAnywhere, Category = Config)
		TSubclassOf<AWeapon> DefaultWeaponClass;


public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;

	UFUNCTION(BlueprintCallable)
		FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	UFUNCTION(BlueprintCallable)
		AEdge_HUD* GetEdgeHUD() { return HUD; }

	UFUNCTION(BlueprintCallable)
		USpringArmComponent* GetSpringarm() { return CameraBoom; }

};

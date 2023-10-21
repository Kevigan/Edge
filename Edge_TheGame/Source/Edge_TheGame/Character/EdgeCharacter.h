// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Edge_TheGame/EdgeTypes/TurningInPlace.h"
#include "Edge_TheGame/Interfaces/InteractWithCrosshairsInterface.h"
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
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	virtual void OnRep_ReplicatedMovement() override;

	void Elim();
	UFUNCTION(NetMulticast, Reliable)
		void MulticastElim();


protected:
	virtual void BeginPlay() override;

	void MoveFoward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void UpdateHUDHealth();
	void PlayHitUI();
	UFUNCTION()
		void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

private:
	class AEdge_HUD* HUD;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* OverHeadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;

	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
		class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
		void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* ElimMontage;

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

	class AEdgePlayerController* EdgePlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;
	void ElimTimerFinished();
	UPROPERTY(EditDefaultsOnly)
		float ElimDelay = 3.f;

public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
};

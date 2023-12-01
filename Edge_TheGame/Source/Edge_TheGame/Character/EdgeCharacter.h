// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Edge_TheGame/EdgeTypes/TurningInPlace.h"
#include "Edge_TheGame/Interfaces/InteractWithCrosshairsInterface.h"
#include "Edge_TheGame/EdgeTypes/CombatState.h"
#include "Edge_TheGame/EdgeTypes/Team.h"
#include "EdgeCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);
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
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void Destroyed() override;

	/*
	* Play Montages
	*/
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlaySwapMontage();

	void Elim(bool bPlayerLeftGame);
	UFUNCTION(NetMulticast, Reliable)
		void MulticastElim(bool bPlayerLeftGame);

	UPROPERTY()
		class AEdgePlayerState* EdgePlayerState = nullptr;

	UPROPERTY(Replicated)
		bool bDisableGameplay = false;

	//Change crosshair color on hit and on death
	void ChangeCrosshairColor(float EnemyHealth, AEdgeCharacter* DamagedCharacter, float DamageNum);

	UFUNCTION(Client, Reliable)
		void ClientChangeCrosshairColor(float EnemyHealth, AEdgeCharacter* DamagedCharacter, float DamageNum);

	FTimerHandle CrosshairTimer;
	void CrosshairTimerFinished();

	UFUNCTION(BlueprintImplementableEvent)
		void ShowSniperScopeWidget(bool bShowScope);

	UFUNCTION(BlueprintImplementableEvent)
		void HideShowSniperScopeWidget();

	void SpawnDefaultWeapon();

	/*
	*  Hit boxes used for server-side rewind
	*/

	UPROPERTY(EditAnywhere)
		class UBoxComponent* head;

	UPROPERTY(EditAnywhere)
		UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
		UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* backpack;

	UPROPERTY(EditAnywhere)
		UBoxComponent* blanket;

	UPROPERTY(EditAnywhere)
		UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* foot_r;

	UPROPERTY()
		TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	bool bFinishSwapping = false;
	bool bJustFired = false;
	bool bFireButtonPressed = false;
	FTimerHandle FireTimerFirePressed;
	void StartFireTimer();
	void FireTimerFinished();

	bool bJustEquipped = false;
	FTimerHandle EquipTimerFire;
	void StartEquipTimer();
	void EquipTimerFinished();

	UFUNCTION(Server, Reliable)
		void ServerLeaveGame();

	FOnLeftGame OnLeftGame;

	void SetTeamColor(ETeam Team);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastTabPressed(const TArray<class AEdgePlayerState*>& ServerRedTeam, const TArray< AEdgePlayerState*>& ServerBlueTeam);

	UPROPERTY(BlueprintReadWrite)
		int32 InvertedLookMultiplyer = 1;

	UPROPERTY(BlueprintReadWrite)
		float MouseSensitivity = 1.f;

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnReload();

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnReloadFinish();

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnElimmed();

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
	void TabButtonPressed();
	void TabButtonReleased();
	void SprintButtonPressed();
	void SprintButtonReleased();
	void StealthModeButtonPressed();
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	virtual void Jump() override;
	void FireButtonPressed();
	void FireButtonReleased();
	void UpdateHUDHealth();
	void UpdateHUDAmmo();
	void PlayHitUI();
	void DropOrDestroyWeapon(AWeapon* Weapon);
	void DropOrDestroyWeapons();
	void SetSpawnPoint();
	void OnPlayerStateInitialized();

	UFUNCTION(Server, Reliable)
		void ServerTabPressed();

	UPROPERTY(BlueprintReadWrite)
		TArray<AEdgePlayerState*> RedTeam;

	UPROPERTY(BlueprintReadWrite)
		TArray<AEdgePlayerState*> BlueTeam;

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

	UPROPERTY()
		AActor* MiniMapActor = nullptr;

	UPROPERTY(EditAnywhere, Category = Config)
		TSubclassOf<class AActor> PlayerIndicatorClass;

	UPROPERTY()
		AActor* PlayerIndicatorActor = nullptr;

	UFUNCTION()
		void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	void RotateInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Config)
		float SprintSpeed = 750;

	float EdgeWalkSpeed;

	bool bCanCoyoteJump = false;
	FTimerHandle CoyoteJumpTimer;
	void CoyoteJumpFinished();
	UPROPERTY(EditAnywhere, Category = Config = Movement)
		float CoyoteJumpDelay = 0.2f;

	UFUNCTION(Server, Reliable)
		void Server_StealthMode();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_StealthMode();

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnStealthMode();

private:
	class AEdge_HUD* HUD;

	/*
	* Edge components
	*/
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* OverHeadWidget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = Camera)
		class ULagCompensationComponent* LagCompensation;

	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* Combat = nullptr;

	UFUNCTION(Server, Reliable)
		void ServerEquipButtonPressed(AWeapon* OverlappedWeapon);

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

	UPROPERTY(EditAnywhere, Category = Config = Combat)
		class UAnimMontage* SwapMontage = nullptr;

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

	bool bLeftGame = false;

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

	UPROPERTY(EditAnywhere, Category = Config)
		class USoundCue* HitSoundBody;

	UPROPERTY(EditAnywhere, Category = Config)
		class USoundCue* DeathSound;

	/*
	* Team Colors
	*/

	UPROPERTY(EditAnywhere, Category = Config)
		UMaterialInstance* RedTeamMat;

	UPROPERTY(EditAnywhere, Category = Config)
		UMaterialInstance* BlueTeamMat;

	UPROPERTY(EditAnywhere, Category = Config)
		UMaterialInstance* NoTeamMat;

	UPROPERTY(EditAnywhere)
		class AEdgeGameMode* EdgeGameMode;

	UFUNCTION(Server, Reliable)
		void Server_ChangeSkin(const FString& Skin);

	

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
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }
	UFUNCTION(BlueprintCallable)
		AEdge_HUD* GetEdgeHUD() { return HUD; }

	UFUNCTION(BlueprintCallable)
		USpringArmComponent* GetSpringarm() { return CameraBoom; }
	bool IsLocallyReloading();
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgeCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "Edge_TheGame/Weapon/Weapon.h"
#include "Edge_TheGame/EdgeComponents/CombatComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Edge_TheGame/Character/EdgeAnimInstance.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Edge_TheGame/HUD/Edge_HUD.h"
#include "Kismet/GameplayStatics.h"
#include "Edge_TheGame/Edge_TheGame.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Edge_TheGame/GameMode/EdgeGameMode.h"
#include "TimerManager.h"
#include "Edge_TheGame/PlayerState/EdgePlayerState.h"
#include "Edge_TheGame/Weapon/WeaponTypes.h"
#include "Edge_TheGame/EdgeComponents/LagCompensationComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/BoxComponent.h"

AEdgeCharacter::AEdgeCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverHeadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverHeadWidget"));
	OverHeadWidget->SetupAttachment(RootComponent);

	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagComponent"));

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 720.f);

	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	/*
	*  Hit boxes for server-side rewind
	*/

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("backpack"), backpack);

	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("blanket"), blanket);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AEdgeCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AEdgeCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(AEdgeCharacter, Health);
	DOREPLIFETIME(AEdgeCharacter, bDisableGameplay);
}

void AEdgeCharacter::BeginPlay()
{
	Super::BeginPlay();
	//SpawnDefaultWeapon();
	UpdateHUDAmmo();
	UpdateHUDHealth();
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ThisClass::ReceiveDamage);
	}
	//SpawnPlayerIndicator();
	SpawnMiniMapOnBeginPlay();
}

void AEdgeCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RotateInPlace(DeltaTime);
	HideCameraIfCharacterClose();
	PollInit();
	if (GEngine && Combat)
	{
		GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Red, FString::Printf(TEXT("CombatState: %s"), *UEnum::GetValueAsString(Combat->CombatState)));
	}
	if (PlayerIndicatorActor)
	{
		PlayerIndicatorActor->SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 1000.f));

	}
	if (MiniMapActor)
	{
		MiniMapActor->SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 10000.f));
		MiniMapActor->SetActorRotation(FRotator(0.f, CameraBoom->GetTargetRotation().Yaw, 0.f));
	}
}

void AEdgeCharacter::RotateInPlace(float DeltaTime)
{
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

void AEdgeCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ThisClass::Jump);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ThisClass::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ThisClass::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ThisClass::ReloadButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ThisClass::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ThisClass::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ThisClass::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ThisClass::FireButtonReleased);
	PlayerInputComponent->BindAction("ESC", IE_Pressed, this, &ThisClass::EscapeButtonPressed);
	PlayerInputComponent->BindAction("SwapWeapons", IE_Pressed, this, &ThisClass::MouseWheelTurned);
	PlayerInputComponent->BindAction("ShowTeamData", IE_Pressed, this, &ThisClass::TabButtonPressed);

	PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveFoward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AEdgeCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<AEdgePlayerController>(Controller);
		}
	}
}

void AEdgeCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

void AEdgeCharacter::ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
	UpdateHUDHealth();
	PlayHitUI();
	// On hit change crosshair color

	ACharacter* OwnerCharacter = Cast<ACharacter>(InstigatorController->GetCharacter());
	if (OwnerCharacter && Health > 0.f)
	{
		AEdgeCharacter* EdgeCharacterEnemy = Cast<AEdgeCharacter>(OwnerCharacter);
		if (EdgeCharacterEnemy)
		{

			if (!EdgeCharacterEnemy->IsLocallyControlled())
			{
				EdgeCharacterEnemy->ClientChangeCrosshairColor(Health);
			}
			else
			{
				EdgeCharacterEnemy->ChangeCrosshairColor(Health);
			}
		}

	}
	if (Health == 0.f)
	{
		AEdgeGameMode* EdgeGameMode = GetWorld()->GetAuthGameMode<AEdgeGameMode>();
		if (EdgeGameMode)
		{
			EdgePlayerController = EdgePlayerController == nullptr ? Cast<AEdgePlayerController>(Controller) : EdgePlayerController;
			AEdgePlayerController* AttackerController = Cast<AEdgePlayerController>(InstigatorController);
			EdgeGameMode->PlayerEliminated(this, EdgePlayerController, AttackerController);
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString(TEXT("freeeeeeeeeeeee3!")));
			// 
			//On Death change crosshair color
			if (OwnerCharacter)
			{
				AEdgeCharacter* EdgeCharacterEnemy = Cast<AEdgeCharacter>(OwnerCharacter);
				if (EdgeCharacterEnemy)
				{
					if (!EdgeCharacterEnemy->IsLocallyControlled())
					{
						EdgeCharacterEnemy->ClientChangeCrosshairColor(Health);
						EdgeCharacterEnemy->ClientAddKillText(this);
					}
					else
					{
						EdgeCharacterEnemy->ChangeCrosshairColor(Health);
						EdgeCharacterEnemy->AddKillText(this);
					}
				}
			}
		}
	}
}

void AEdgeCharacter::PlayFireMontage(bool bAiming)
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AEdgeCharacter::PlayReloadMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaulRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

void AEdgeCharacter::Elim()
{
	DropOrDestroyWeapons();
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ThisClass::ElimTimerFinished,
		ElimDelay
	);
}

void AEdgeCharacter::MulticastElim_Implementation()
{
	if (EdgePlayerController)
	{
		EdgePlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();

	//Disable character movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	bDisableGameplay = true;

	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}

	//Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bool bHideSniperScope = IsLocallyControlled() && Combat && Combat->bAiming && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
}

void AEdgeCharacter::ElimTimerFinished()
{
	AEdgeGameMode* EdgeGameMode = GetWorld()->GetAuthGameMode<AEdgeGameMode>();
	if (EdgeGameMode)
	{
		EdgeGameMode->RequestRespawn(this, Controller);
	}
}

void AEdgeCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	if (Weapon == nullptr) return;
	if (Weapon->bDestroyWeapon)
	{
		Weapon->Destroy();
	}
	else
	{
		Weapon->Dropped();
	}
}

void AEdgeCharacter::DropOrDestroyWeapons()
{
	if (Combat)
	{
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
	}
}

void AEdgeCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

void AEdgeCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

void AEdgeCharacter::PlayHitUI()
{
	if (!IsLocallyControlled()) return;
	if (Combat)
	{
		//Combat->SetCrossHairCOlor(FLinearColor::Blue);
	}
	HUD = HUD == nullptr ? Cast<AEdge_HUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()) : HUD;
	if (HUD)
	{
		HUD->ReceiveOnShowHitUI();
	}
}

void AEdgeCharacter::SpawnPlayerIndicator_Implementation()
{
	ReceiveOnSpawnPlayerIndicator();
}

void AEdgeCharacter::SpawnMiniMapOnBeginPlay()
{
	GetWorldTimerManager().SetTimer(
		InitializeMiniMapTimer,
		this,
		&ThisClass::InitializeMiniMapTimerFinished,
		1.f
	);
}

void AEdgeCharacter::SpawnMiniMapServer_Implementation()
{
	SpawnMiniMap();
}

void AEdgeCharacter::SpawnMiniMap()
{
	UWorld* World = GetWorld();
	if (World && MiniMapClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		MiniMapActor = World->SpawnActor<AActor>(
			MiniMapClass,
			FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 10000.f),
			FRotator(0.f, 0.f, 0.f),
			SpawnParams
		);
		if (MiniMapActor)
		{
			//MiniMapActor->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
			//MiniMapActor->AttachToComponent(CameraBoom, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
			//MiniMapActor->SetActorRelativeLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 100.f));
		}
	}

	HUD = HUD == nullptr ? Cast<AEdge_HUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()) : HUD;
	if (HUD)
	{
		HUD->AddMiniMap();
	}
	//UWorld* World = GetWorld();
	if (World && PlayerIndicatorClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		PlayerIndicatorActor = World->SpawnActor<AActor>(
			PlayerIndicatorClass,
			GetActorLocation(),
			FRotator(0.f, 0.f, 0.f),
			SpawnParams
		);
		if (PlayerIndicatorActor)
		{
			//PlayerIndicatorActor->Owner = this;
			PlayerIndicatorActor->SetOwner(this);
		}
	}
}

void AEdgeCharacter::ClientSpawnMiniMap_Implementation()
{
	//MulticastTestSpawn();
	//ReceiveOnSpawnPlayerIndicator();
	if (!IsLocallyControlled()) return;
	SpawnMiniMap();

}

void AEdgeCharacter::InitializeMiniMapTimerFinished()
{
	if (HasAuthority() && IsLocallyControlled())
	{
		SpawnMiniMapServer();
	}
	else if (!HasAuthority() && IsLocallyControlled())
	{
		ClientSpawnMiniMap();
	}
}

void AEdgeCharacter::MoveFoward(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));
		AddMovementInput(Direction, Value);
	}
}

void AEdgeCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;
	if (Controller != nullptr && Value != 0)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));
		AddMovementInput(Direction, Value);
	}
}

void AEdgeCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
	ControllerYaw = Value;
}

void AEdgeCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AEdgeCharacter::MouseWheelTurned()
{
	if (bDisableGameplay || Combat == nullptr || Combat->CombatState != ECombatState::ECS_Unoccupied || !Combat->ShouldSwapWeapons()) return;
	if (Combat->SecondaryWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle && Combat->bAiming)
	{
		ShowSniperScopeWidget(true);
	}
	else if (Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle && Combat->SecondaryWeapon->GetWeaponType() != EWeaponType::EWT_SniperRifle && Combat->bAiming)
	{
		ShowSniperScopeWidget(false);
	}
	if (Combat->ShouldSwapWeapons())
	{
		ServerMouseWheelTurned();
	}
	if (Combat->ShouldSwapWeapons() && !HasAuthority() && OverlappingWeapon == nullptr)
	{
		PlaySwapMontage();
		Combat->CombatState = ECombatState::ECS_SwappingWeapons;
		bFinishSwapping = false;
	}
}

void AEdgeCharacter::ServerMouseWheelTurned_Implementation()
{

	if (Combat && Combat->ShouldSwapWeapons())
	{
		Combat->SwapWeapons();
	}
}

void AEdgeCharacter::EquipButtonPressed()
{
	if (bDisableGameplay || Combat == nullptr || Combat->CombatState != ECombatState::ECS_Unoccupied) return;
	if (Combat->ShouldSwapWeapons())
	{
		if (Combat->SecondaryWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle && Combat->bAiming)
		{
			ShowSniperScopeWidget(true);
		}
		else if (Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle && Combat->SecondaryWeapon->GetWeaponType() != EWeaponType::EWT_SniperRifle && Combat->bAiming)
		{
			ShowSniperScopeWidget(false);
		}
	}

	if (Combat)
	{
		if (Combat->CombatState == ECombatState::ECS_Unoccupied) ServerEquipButtonPressed();
		if (Combat->ShouldSwapWeapons() && !HasAuthority() && OverlappingWeapon == nullptr)
		{
			PlaySwapMontage();
			Combat->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishSwapping = false;
		}
	}

}

void AEdgeCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapons()) // Make new funtion for using mouse wheel
		{

			Combat->SwapWeapons();
		}
	}
}

void AEdgeCharacter::CrouchButtonPressed()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void AEdgeCharacter::ReloadButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->Reload();
	}
}

void AEdgeCharacter::AimButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void AEdgeCharacter::AimButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void AEdgeCharacter::TabButtonPressed()
{
	EdgePlayerController = EdgePlayerController == nullptr ? Cast<AEdgePlayerController>(Controller) : EdgePlayerController;
	if (EdgePlayerController)
	{
		EdgePlayerController->ShowTeamData();
	}
}

float AEdgeCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();

}

void AEdgeCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;

	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (Speed == 0.f && !bIsInAir)
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}
	if (Speed > 0.f || bIsInAir)
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

void AEdgeCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		//map pitch from [270, 360) to -90, 0)
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AEdgeCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	bRotateRootBone = false;
	float Speed = CalculateSpeed();
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationsLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationsLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}

void AEdgeCharacter::Jump()
{
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Super::Jump();
	}
}

void AEdgeCharacter::FireButtonPressed()
{
	if (bDisableGameplay) return;
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->FireButtonPressed(true);
	}
}

void AEdgeCharacter::FireButtonReleased()
{
	if (bDisableGameplay) return;
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
}

void AEdgeCharacter::EscapeButtonPressed()
{
	EdgePlayerController = EdgePlayerController == nullptr ? Cast<AEdgePlayerController>(Controller) : EdgePlayerController;
	if (EdgePlayerController)
	{
		EdgePlayerController->OpenMenu();

	}
}

void AEdgeCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}

}

void AEdgeCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}

void AEdgeCharacter::OnRep_Health()
{
	UpdateHUDHealth();
	PlayHitUI();
}

void AEdgeCharacter::UpdateHUDHealth()
{
	EdgePlayerController = EdgePlayerController == nullptr ? Cast<AEdgePlayerController>(Controller) : EdgePlayerController;
	if (EdgePlayerController)
	{
		EdgePlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

void AEdgeCharacter::UpdateHUDAmmo()
{
	EdgePlayerController = EdgePlayerController == nullptr ? Cast<AEdgePlayerController>(Controller) : EdgePlayerController;
	if (EdgePlayerController && Combat && Combat->EquippedWeapon)
	{
		EdgePlayerController->SetHUDCarriedWeaponAmmo(Combat->CarriedAmmo);
		EdgePlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

void AEdgeCharacter::PollInit()
{
	if (EdgePlayerState == nullptr)
	{
		EdgePlayerState = GetPlayerState<AEdgePlayerState>();
		if (EdgePlayerState)
		{
			EdgePlayerState->AddToScore(0.f);
			EdgePlayerState->AddToDeath(0);
		}
	}
	if (EdgePlayerController == nullptr)
	{
		EdgePlayerController = EdgePlayerController == nullptr ? Cast<AEdgePlayerController>(Controller) : EdgePlayerController;
		if (EdgePlayerController)
		{
			SpawnDefaultWeapon();
			//ServerSpawnDefaultWeapon();
			UpdateHUDAmmo();
			UpdateHUDHealth();
		}
	}
}

void AEdgeCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{

		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void AEdgeCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);

	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AEdgeCharacter::ChangeCrosshairColor(float EnemyHealth)
{
	FColor Color = FColor::White;
	float Time = 0.f;

	if (EnemyHealth > 0.f)
	{
		Color = FColor::Purple;
		Time = 0.3f;
	}
	else
	{
		Color = FColor::Red;
		Time = 0.8f;

	}

	if (Combat && Combat->EquippedWeapon)
	{
		Combat->ColorToChange = Color;
	}
	GetWorldTimerManager().SetTimer(
		CrosshairTimer,
		this,
		&ThisClass::CrosshairTimerFinished,
		Time
	);
}

void AEdgeCharacter::ClientChangeCrosshairColor_Implementation(float EnemyHealth)
{
	ChangeCrosshairColor(EnemyHealth);
	//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("Health: %f"), EnemyHealth));
}

void AEdgeCharacter::CrosshairTimerFinished()
{
	if (Combat && Combat->EquippedWeapon)
	{
		Combat->ColorToChange = FColor::White;
	}
}

void AEdgeCharacter::AddKillText(AEdgeCharacter* EdgeCharacter)
{
	HUD = HUD == nullptr ? Cast<AEdge_HUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()) : HUD;
	if (HUD)
	{
		HUD->AddKillText();
		APlayerState* EnemyPlayerState = EdgeCharacter->GetPlayerState();
		if (EnemyPlayerState)
		{
			HUD->SetEnemyKilledText(EnemyPlayerState->GetPlayerName());
		}

	}
}

void AEdgeCharacter::ClientAddKillText_Implementation(AEdgeCharacter* EdgeCharacter)
{
	HUD = HUD == nullptr ? Cast<AEdge_HUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetHUD()) : HUD;
	if (HUD)
	{
		HUD->AddKillText();
		APlayerState* EnemyPlayerState = EdgeCharacter->GetPlayerState();
		if (EnemyPlayerState)
		{
			HUD->SetEnemyKilledText(EnemyPlayerState->GetPlayerName());
		}
	}
}

void AEdgeCharacter::SpawnDefaultWeapon()
{
	AEdgeGameMode* EdgeGameMode = Cast<AEdgeGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (EdgeGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

void AEdgeCharacter::ServerSpawnDefaultWeapon_Implementation()
{
	AEdgeGameMode* EdgeGameMode = Cast<AEdgeGameMode>(UGameplayStatics::GetGameMode(this));
	UWorld* World = GetWorld();
	if (EdgeGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}

bool AEdgeCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AEdgeCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

AWeapon* AEdgeCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

FVector AEdgeCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

ECombatState AEdgeCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

bool AEdgeCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

void AEdgeCharacter::Destroyed()
{
	Super::Destroyed();

	AEdgeGameMode* EdgeGameMode = Cast<AEdgeGameMode>(UGameplayStatics::GetGameMode(this));
	//Not in use atm
	// 
	//bool bMatchNotInProgress = EdgeGameMode && EdgeGameMode->GetMatchState() != MatchState::InProgress;
	//if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	//{
	//	//Combat->EquippedWeapon->Destroy();
	//}
	//

	if (Combat && Combat->EquippedWeapon)
	{
		Combat->EquippedWeapon->Dropped();
	}
	if (MiniMapActor != nullptr)
	{
		MiniMapActor->Destroy();
		MiniMapActor = nullptr;
	}
	if (PlayerIndicatorActor != nullptr)
	{
		PlayerIndicatorActor->Destroy();
		PlayerIndicatorActor = nullptr;
	}
}






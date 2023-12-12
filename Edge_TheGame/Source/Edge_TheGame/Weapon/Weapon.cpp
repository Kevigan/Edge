// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Edge_TheGame/EdgeGameInstance.h"
#include "Kismet/GameplayStatics.h"



AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled((ECollisionEnabled::NoCollision));

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}




void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponState = EWeaponState::EWS_Initial;

	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, bUseServerSideRewind, COND_OwnerOnly);
}


void AWeapon::Server_ChangeSkin_Implementation(const FString& Skin)
{
	Multicast_ChangeSKin(Skin);
	ReceiveTEST();
}

void AWeapon::Multicast_ChangeSKin_Implementation(const FString& Skin)
{
	ReceiveOnEquipped(Skin);
}

void AWeapon::Test(AEdgeCharacter* OwnerCHar)
{
	if (OwnerCHar && OwnerCHar->IsLocallyControlled())
	{
		Server_ChangeSkin(GetCurrentSkinWeaponType());
	}
}

FString AWeapon::GetCurrentSkinWeaponType()
{
	EdgeGameInstance = EdgeGameInstance == nullptr ? Cast<UEdgeGameInstance>(UGameplayStatics::GetGameInstance(this)) : EdgeGameInstance;
	if (EdgeGameInstance)
	{
		switch (WeaponType)
		{
		case EWeaponType::EWT_Pistol:
			return EdgeGameInstance->CurrentPistolString;
			break;
		case EWeaponType::EWT_Shotgun:
			return EdgeGameInstance->CurrentShotgunString;
			break;
		case EWeaponType::EWT_AssaulRifle:
			return EdgeGameInstance->CurrentRifleString;
			break;
		case EWeaponType::EWT_SniperRifle:
			return EdgeGameInstance->CurrentSniperString;
			break;
		}
	}
	return FString();
}

void AWeapon::Client_SpawnWeapon_Implementation()
{
	Server_SpawnWeapon();
}

void AWeapon::Server_SpawnWeapon_Implementation()
{
	Multicast_SpawnWeapon();
}


void AWeapon::Multicast_SpawnWeapon_Implementation()
{
	ReceiveOnSpawnWeapon();
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEdgeCharacter* EdgeCharacter = Cast<AEdgeCharacter>(OtherActor);
	if (EdgeCharacter && PickupWidget)
	{
		EdgeCharacter->SetOverlappingWeapon(this);

	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AEdgeCharacter* EdgeCharacter = Cast<AEdgeCharacter>(OtherActor);
	if (EdgeCharacter && PickupWidget)
	{
		EdgeCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AWeapon::SetHUDAmmo()
{
	EdgeOwnerCharacter = EdgeOwnerCharacter == nullptr ? Cast<AEdgeCharacter>(GetOwner()) : EdgeOwnerCharacter;
	if (EdgeOwnerCharacter)
	{
		EdgeOwnerController = EdgeOwnerController == nullptr ? Cast<AEdgePlayerController>(EdgeOwnerCharacter->Controller) : EdgeOwnerController;
		if (EdgeOwnerController)
		{
			EdgeOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
	if (HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else
	{
		++Sequence;
	}
}

void AWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (HasAuthority()) return;

	Ammo = ServerAmmo;
	--Sequence;
	Ammo -= Sequence;
	SetHUDAmmo();
}

void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	SetHUDAmmo();
	ClientAddAmmo(AmmoToAdd);
}

void AWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	if (HasAuthority()) return;

	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	//EdgeOwnerCharacter = EdgeOwnerCharacter == nullptr ? Cast<AEdgeCharacter>(GetOwner()) : EdgeOwnerCharacter;
	SetHUDAmmo();
}

void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	if (Owner == nullptr)
	{
		EdgeOwnerCharacter = nullptr;
		EdgeOwnerController = nullptr;
	}
	else
	{
		EdgeOwnerCharacter = EdgeOwnerCharacter == nullptr ? Cast<AEdgeCharacter>(Owner) : EdgeOwnerCharacter;
		if (EdgeOwnerCharacter && EdgeOwnerCharacter->GetEquippedWeapon() && EdgeOwnerCharacter->GetEquippedWeapon() == this)
		{
			SetHUDAmmo();
			//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString(TEXT("asdqwe")));
			//ReceiveOnSpawnWeapon();
		}
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;

	OnWeaponStateSet();
}

void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Initial:

		break;
	case EWeaponState::EWS_Equipped:
		OnEquipped();
		break;
	case EWeaponState::EWS_EquippedSecondary:
		OnEquippedSecondary();
		break;
	case EWeaponState::EWS_Dropped:
		OnDropped();
		break;
	case EWeaponState::EWS_MAX:

		break;
	}
}

void AWeapon::OnPingTooHigh(bool bPingTooHigh)
{
	bUseServerSideRewind = !bPingTooHigh;
}

void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

void AWeapon::OnEquipped()
{
	ShowPickupWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EdgeOwnerCharacter = EdgeOwnerCharacter == nullptr ? Cast<AEdgeCharacter>(GetOwner()) : EdgeOwnerCharacter;
	/*if (EdgeOwnerCharacter)
	{
		Server_ChangeSkin(GetCurrentSkinWeaponType());
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString(TEXT("asdqwe")));
	}*/
	if (EdgeOwnerCharacter && bUseServerSideRewind)
	{
		EdgeOwnerController = EdgeOwnerController == nullptr ? Cast<AEdgePlayerController>(EdgeOwnerCharacter->Controller) : EdgeOwnerController;
		if (EdgeOwnerController && HasAuthority() && !EdgeOwnerController->HighPingDelegate.IsBound())
		{
			EdgeOwnerController->HighPingDelegate.AddDynamic(this, &ThisClass::OnPingTooHigh);
		}
	}

	//ReceiveOnEquipped();
}

void AWeapon::OnEquippedSecondary()
{
	ShowPickupWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EdgeOwnerCharacter = EdgeOwnerCharacter == nullptr ? Cast<AEdgeCharacter>(GetOwner()) : EdgeOwnerCharacter;
	/*if (EdgeOwnerCharacter)
	{
		Server_ChangeSkin(GetCurrentSkinWeaponType());
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, FString(TEXT("asdqwe")));
	}*/
	if (EdgeOwnerCharacter && bUseServerSideRewind)
	{
		EdgeOwnerController = EdgeOwnerController == nullptr ? Cast<AEdgePlayerController>(EdgeOwnerCharacter->Controller) : EdgeOwnerController;
		if (EdgeOwnerController && HasAuthority() && EdgeOwnerController->HighPingDelegate.IsBound())
		{
			EdgeOwnerController->HighPingDelegate.RemoveDynamic(this, &ThisClass::OnPingTooHigh);
		}
	}
	//ReceiveOnEquipped();
}

void AWeapon::OnDropped()
{
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	EdgeOwnerCharacter = EdgeOwnerCharacter == nullptr ? Cast<AEdgeCharacter>(GetOwner()) : EdgeOwnerCharacter;
	if (EdgeOwnerCharacter && bUseServerSideRewind)
	{
		EdgeOwnerController = EdgeOwnerController == nullptr ? Cast<AEdgePlayerController>(EdgeOwnerCharacter->Controller) : EdgeOwnerController;
		if (EdgeOwnerController && HasAuthority() && EdgeOwnerController->HighPingDelegate.IsBound())
		{
			EdgeOwnerController->HighPingDelegate.RemoveDynamic(this, &ThisClass::OnPingTooHigh);
		}
	}
}

void AWeapon::ShowPickupWidget(bool bShowWidget)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AWeapon::Fire(const FVector& HitTarget)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
				);
			}
		}
	}
	SpendRound();
}

void AWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);
	SetOwner(nullptr);
	EdgeOwnerCharacter = nullptr;
	EdgeOwnerController = nullptr;
}

bool AWeapon::IsEmpty()
{
	return Ammo <= 0;
}

bool AWeapon::IsFull()
{
	return Ammo == MagCapacity;
}

FVector AWeapon::TraceEndWithScatter(const FVector& HitTarget)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return FVector();

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform((GetWeaponMesh()));
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	const FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	const FVector EndLoc = SphereCenter + RandVector;
	const FVector ToEndLoc = EndLoc - TraceStart;
	/*DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red,true);
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange,true);
	DrawDebugLine(GetWorld(), TraceStart, FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()), FColor::Cyan, true);*/

	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}
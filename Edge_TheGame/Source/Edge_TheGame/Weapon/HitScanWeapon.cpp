// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "WeaponTypes.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform((GetWeaponMesh()));
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		AEdgeCharacter* EdgeCharacter = Cast<AEdgeCharacter>(FireHit.GetActor());
		if (EdgeCharacter && HasAuthority() && InstigatorController)
		{
			UGameplayStatics::ApplyDamage(
				EdgeCharacter,
				Damage,
				InstigatorController,
				this,
				UDamageType::StaticClass()
			);
		}
		if (ImpactParticles)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ImpactParticles,
				FireHit.ImpactPoint,
				FireHit.ImpactNormal.Rotation()
			);
		}
		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				HitSound,
				FireHit.ImpactPoint
			);
		}

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				MuzzleFlash,
				SocketTransform
			);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this, 
				FireSound,
				GetActorLocation()
			);
		}
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = bUseScatter ? TraceEndWithScatter(TraceStart, HitTarget) : TraceStart + (HitTarget - TraceStart) * 1.25f;
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		if (OutHit.bBlockingHit)
		{

		}
	}
}

FVector AHitScanWeapon::TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget)
{
	FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;
	FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
	FVector EndLoc = SphereCenter + RandVector;
	FVector ToEndLoc = EndLoc - TraceStart;
	/*DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red,true);
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange,true);
	DrawDebugLine(GetWorld(), TraceStart, FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()), FColor::Cyan, true);*/

	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}



// Fill out your copyright notice in the Description page of Project Settings.


#include "ShotGun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Edge_TheGame/EdgeComponents/LagCompensationComponent.h"


void AShotGun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform((GetWeaponMesh()));
		const FVector Start = SocketTransform.GetLocation();

		//Maps hit character to number of times hit
		TMap<AEdgeCharacter*, uint32> HitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			AEdgeCharacter* EdgeCharacter = Cast<AEdgeCharacter>(FireHit.GetActor());
			if (EdgeCharacter)
			{
				if (HitMap.Contains(EdgeCharacter))
				{
					HitMap[EdgeCharacter]++;
				}
				else
				{
					HitMap.Emplace(EdgeCharacter, 1);
				}
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
					FireHit.ImpactPoint,
					.5f,
					FMath::FRandRange(-0.5f, .5f)
				);
			}
		}
		TArray<AEdgeCharacter*> HitCharacters;

		for (auto HitPair : HitMap)
		{
			if (HitPair.Key && InstigatorController)
			{
				bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (HasAuthority() && bCauseAuthDamage)
				{
					UGameplayStatics::ApplyDamage(
						HitPair.Key, // character 
						Damage * HitPair.Value, // multiply damage by number of times hit
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}
				HitCharacters.Add(HitPair.Key);
			}
		}
		if (!HasAuthority() && bUseServerSideRewind)
		{
			EdgeOwnerCharacter = EdgeOwnerCharacter == nullptr ? Cast<AEdgeCharacter>(OwnerPawn) : EdgeOwnerCharacter;
			EdgeOwnerController = EdgeOwnerController == nullptr ? Cast<AEdgePlayerController>(InstigatorController) : EdgeOwnerController;
			if (EdgeOwnerController && EdgeOwnerCharacter && EdgeOwnerCharacter->GetLagCompensation() && EdgeOwnerCharacter->IsLocallyControlled())
			{
				EdgeOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
					HitCharacters,
					Start,
					HitTargets,
					EdgeOwnerController->GetServerTime() - EdgeOwnerController->SingleTripTime
				);
			}
		}
	}
}
void AShotGun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr) return;

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform((GetWeaponMesh()));
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVector;
		FVector ToEndLoc = EndLoc - TraceStart;
		ToEndLoc = TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size();

		HitTargets.Add(ToEndLoc);
	}
}


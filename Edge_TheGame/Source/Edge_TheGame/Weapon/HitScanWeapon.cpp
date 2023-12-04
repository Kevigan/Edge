// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "WeaponTypes.h"
#include "particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Edge_TheGame/EdgeComponents/LagCompensationComponent.h"
#include "Edge_TheGame/Xylofon.h"
//#include "NiagaraFunctionLibrary.h"
//#include "NiagaraComponent.h"


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
		if (EdgeCharacter && InstigatorController)
		{
			bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
			if (HasAuthority() && bCauseAuthDamage)
			{
				UGameplayStatics::ApplyDamage(
					EdgeCharacter,
					Damage,
					InstigatorController,
					this,
					UDamageType::StaticClass()
				);
			}
			if (!HasAuthority() && bUseServerSideRewind)
			{
				EdgeOwnerCharacter = EdgeOwnerCharacter == nullptr ? Cast<AEdgeCharacter>(OwnerPawn) : EdgeOwnerCharacter;
				EdgeOwnerController = EdgeOwnerController == nullptr ? Cast<AEdgePlayerController>(InstigatorController) : EdgeOwnerController;
				if (EdgeOwnerController && EdgeOwnerCharacter && EdgeOwnerCharacter->GetLagCompensation() && EdgeOwnerCharacter->IsLocallyControlled())
				{
					EdgeOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
						EdgeCharacter,
						Start,
						HitTarget,
						EdgeOwnerController->GetServerTime() - EdgeOwnerController->SingleTripTime,
						this
					);
				}
			}
		}
		if (ImpactParticles)
		{
			if (FireHit.GetActor() && FireHit.GetActor()->IsA<AEdgeCharacter>())
			{
				ReceiveOnHitBody(FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
			}
			else
			{
				UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(),
					ImpactParticles,
					FireHit.ImpactPoint,
					FireHit.ImpactNormal.Rotation()
				);
			}

		}

		if (FireHit.GetActor() && FireHit.GetActor()->IsA<AXylofon>())
		{
			EdgeOwnerCharacter = EdgeOwnerCharacter == nullptr ? Cast<AEdgeCharacter>(OwnerPawn) : EdgeOwnerCharacter;
			AXylofon* Xylofon = Cast<AXylofon>(FireHit.GetActor());
			if (EdgeOwnerCharacter)
			{
				Xylofon->PlaySoundLocal(EdgeOwnerCharacter);
				Xylofon->Server_PlayXylofonSound(EdgeOwnerCharacter);
			}
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
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility
		);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}

		//DrawDebugSphere(GetWorld(), BeamEnd, 16.f, 12, FColor::Orange, true);
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString(TEXT("Hit")));
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
				World,
				BeamParticles,
				TraceStart,
				FRotator::ZeroRotator,
				true
			);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
	}
}





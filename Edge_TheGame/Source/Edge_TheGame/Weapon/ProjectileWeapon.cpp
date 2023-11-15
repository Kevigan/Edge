// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AProjectileWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	UWorld* World = GetWorld();
	if (MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		// From muzzleflashsocket to hitlocation from trace under crosshairs
		FVector ToTarget = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = InstigatorPawn;

		AProjectile* SpawnedProjectile = nullptr;
		if (bUseServerSideRewind)
		{
			if (InstigatorPawn != nullptr && InstigatorPawn->HasAuthority()) // Server
			{
				if (InstigatorPawn->IsLocallyControlled()) // Server, host - use replicated projectile
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					if (SpawnedProjectile != nullptr)
					{
						SpawnedProjectile->bUseServerSideRewind = false;
						SpawnedProjectile->Damage = Damage;
					}
				}
				else // Server, not locally controlled - spawn non replicated projectile, ssr
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					if (SpawnedProjectile != nullptr)
					{
						SpawnedProjectile->bUseServerSideRewind = true;
					}
				}
			}
			else // CLient, using ssr
			{
				if (InstigatorPawn != nullptr && InstigatorPawn->IsLocallyControlled()) // Client, locally controlled - spawn non replicated projectile, use ssr
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					if (SpawnedProjectile != nullptr)
					{
						SpawnedProjectile->bUseServerSideRewind = true;
						SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
						SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
						SpawnedProjectile->Damage = Damage;
					}
				}
				else // Client, not locally controlled, spawn non replicated projectile, no ssr
				{
					SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
					if (SpawnedProjectile != nullptr)
					{
						SpawnedProjectile->bUseServerSideRewind = false;
					}
				}
			}
		}
		else // Weapon not using ssr
		{
			if (InstigatorPawn != nullptr && InstigatorPawn->HasAuthority())
			{
				SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
				if (SpawnedProjectile != nullptr)
				{
					SpawnedProjectile->bUseServerSideRewind = false;
					SpawnedProjectile->Damage = Damage;
				}
			}
		}
	}
}



#include "WeaponSpawner.h"
#include "Edge_TheGame/Weapon/WeaponTypes.h"

AWeaponSpawner::AWeaponSpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AWeaponSpawner::BeginPlay()
{
	Super::BeginPlay();

}

void AWeaponSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponSpawner::Server_SpawnWeapon_Implementation(EWeaponType WeaponType)
{
	Multicast_SpawnWeapon(WeaponType);
}


void AWeaponSpawner::Multicast_SpawnWeapon_Implementation(EWeaponType WeaponType)
{
	ReceiveOnSpawnWeapon(WeaponType);
}



// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Edge_TheGame/Weapon/WeaponTypes.h"
#include "WeaponSpawner.generated.h"

UCLASS()
class EDGE_THEGAME_API AWeaponSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponSpawner();

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_SpawnWeapon(EWeaponType WeaponType);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void ReceiveOnSpawnWeapon(EWeaponType WeaponType);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_SpawnWeapon(EWeaponType WeaponType);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

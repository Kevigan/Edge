// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Xylofon.generated.h"

UCLASS()
class EDGE_THEGAME_API AXylofon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AXylofon();
	virtual void Tick(float DeltaTime) override;

	void PlaySoundLocal(class AEdgeCharacter* EdgeCharacter);

	UFUNCTION(Server, Reliable)
		void Server_PlayXylofonSound(AEdgeCharacter* EdgeCharacter);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_PlayXylofonSound(AEdgeCharacter* EdgeCharacter);


protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void ReceivePlayXylofonSound(AEdgeCharacter* EdgeCharacter);

	UFUNCTION(BlueprintImplementableEvent)
		void ReceivePlayXylofonLocalSound(AEdgeCharacter* EdgeCharacter);
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Xylofon.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"

// Sets default values
AXylofon::AXylofon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AXylofon::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AXylofon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AXylofon::PlaySoundLocal(AEdgeCharacter* EdgeCharacter)
{
	if (EdgeCharacter == nullptr) return;
	ReceivePlayXylofonLocalSound(EdgeCharacter);
}

void AXylofon::Server_PlayXylofonSound_Implementation(AEdgeCharacter* EdgeCharacter)
{
	if (EdgeCharacter == nullptr) return;
	Multicast_PlayXylofonSound(EdgeCharacter);
}

void AXylofon::Multicast_PlayXylofonSound_Implementation(AEdgeCharacter* EdgeCharacter)
{
	if (EdgeCharacter == nullptr) return;
	ReceivePlayXylofonSound(EdgeCharacter);
}


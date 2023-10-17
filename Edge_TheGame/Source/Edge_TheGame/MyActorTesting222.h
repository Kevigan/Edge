// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActorTesting222.generated.h"

UCLASS()
class EDGE_THEGAME_API AMyActorTesting222 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActorTesting222();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void test();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

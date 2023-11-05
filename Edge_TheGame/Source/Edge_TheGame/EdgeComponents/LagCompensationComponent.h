// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

		UPROPERTY()
		FVector Location;

	UPROPERTY()
		FRotator Rotation;

	UPROPERTY()
		FVector BoxExtent;
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

		UPROPERTY()
		float Time;

	UPROPERTY()
		TMap<FName, FBoxInformation> HitboxInfo;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EDGE_THEGAME_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULagCompensationComponent();
	friend class AEdgeCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& Package, const FColor Color);

protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);

private:

	UPROPERTY()
		AEdgeCharacter* Character;

	UPROPERTY()
		class AEdgePlayerController* Controller;

public:


};

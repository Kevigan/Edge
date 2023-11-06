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

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
		bool bHitConfirmed;

	UPROPERTY()
		bool bHeadShot;
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
	FServerSideRewindResult ServerSideRewind(AEdgeCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);

protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);
	FServerSideRewindResult ConfirmHit(const FFramePackage& Package, AEdgeCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);
	void CacheBoxPositions(AEdgeCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(AEdgeCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AEdgeCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(AEdgeCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnable);

private:

	UPROPERTY()
		AEdgeCharacter* Character;

	UPROPERTY()
		class AEdgePlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere, Category = Config)
		float MaxRecordTime = 4.f;

public:


};

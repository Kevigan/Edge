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

	UPROPERTY()
		AEdgeCharacter* Character;
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

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

		UPROPERTY()
		TMap<AEdgeCharacter*, uint32> HeadShots;
	UPROPERTY()
		TMap<AEdgeCharacter*, uint32> BodyShots;

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

	/*
	* HitScan
	*/
	FServerSideRewindResult ServerSideRewind(AEdgeCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime);

	/*
	* Projectile
	*/
	FServerSideRewindResult ProjectileServerSideRewind(AEdgeCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime);

	/*
	* Shotgun
	*/
	FShotgunServerSideRewindResult ShotgunServerSideRewind(const TArray<AEdgeCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime);

	UFUNCTION(Server, Reliable)
		void ServerScoreRequest(AEdgeCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, class AWeapon* DamageCauser);

	UFUNCTION(Server, Reliable)
		void ProjectileServerScoreRequest(AEdgeCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime);

	UFUNCTION(Server, Reliable)
		void ShotgunServerScoreRequest(const TArray<AEdgeCharacter*>& HitCharacters,
			const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime);

		bool bDrawDebug = false;
protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& Package);
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);

	void CacheBoxPositions(AEdgeCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(AEdgeCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(AEdgeCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(AEdgeCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnable);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(AEdgeCharacter* HitCharacter, float HitTime);

	/*
	* HitScan
	*/
	FServerSideRewindResult ConfirmHit(const FFramePackage& Package, AEdgeCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation);

	/*
	* Projectile
	*/
	FServerSideRewindResult ProjectileConfirmHit(const FFramePackage& Package, AEdgeCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime);

	/*
	* Shotgun
	*/
	FShotgunServerSideRewindResult ShotgunConfirmHit(const TArray<FFramePackage>& FramePackages, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations);


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

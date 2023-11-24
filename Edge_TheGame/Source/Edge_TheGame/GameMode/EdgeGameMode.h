// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EdgeGameMode.generated.h"

namespace MatchState
{
	extern EDGE_THEGAME_API const FName Cooldown; // Match duration has been reached, display winner and begin cooldown timer
}

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API AEdgeGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AEdgeGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(class AEdgeCharacter* ElimmedCharacter, class AEdgePlayerController* VictimController, class AEdgePlayerController* AttackerContoller);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ELimmedController);
	void PlayerLeftGame(class AEdgePlayerState* PlayerLeaving);

	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);

	UPROPERTY(EditDefaultsOnly)
		float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
		float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
		float CooldownTime = 10.f;

	float LevelStartingTime = 0.f;

	bool bTeamsMatch = false;

	void FinishGame();
protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	float CountDownTime = 0.f;

public:
	FORCEINLINE float GetCountdownTime() const {return CountDownTime;}
};

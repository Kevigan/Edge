// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdgeGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class EDGE_THEGAME_API ATeamsGameMode : public AEdgeGameMode
{
	GENERATED_BODY()
	
public:
	ATeamsGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	virtual void PlayerEliminated(class AEdgeCharacter* ElimmedCharacter, class AEdgePlayerController* VictimController, class AEdgePlayerController* AttackerContoller) override;

protected:
	virtual void HandleMatchHasStarted() override;
};

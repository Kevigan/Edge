// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EdgeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class EDGE_THEGAME_API AEdgeGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PlayerEliminated(class AEdgeCharacter* ElimmedCharacter, class AEdgePlayerController* VictimController, class AEdgePlayerController* AttackerContoller);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ELimmedController);
};

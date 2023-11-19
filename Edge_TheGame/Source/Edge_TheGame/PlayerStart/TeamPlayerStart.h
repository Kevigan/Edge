// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Edge_TheGame/EdgeTypes/Team.h"
#include "TeamPlayerStart.generated.h"

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API ATeamPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Config)
		ETeam Team;
};

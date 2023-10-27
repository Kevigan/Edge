// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	UPROPERTY(BlueprintReadWrite, Category = Config)
		FString ChosenLevelPath = " /Game/Maps/Level_1?listen";

	FTimerHandle BeginTraverToLevelTimer;
	void BeginTraverToLevelTimerFinished();
private:


};

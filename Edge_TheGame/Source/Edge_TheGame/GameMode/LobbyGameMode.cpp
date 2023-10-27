// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers == 1)
	{
		GetWorldTimerManager().SetTimer(
			BeginTraverToLevelTimer,
			this,
			&ThisClass::BeginTraverToLevelTimerFinished,
			10.f);
	}
}

void ALobbyGameMode::BeginTraverToLevelTimerFinished()
{
	UWorld* World = GetWorld();
	if (World && !ChosenLevelPath.IsEmpty())
	{
		bUseSeamlessTravel = true;
		//World->ServerTravel(FString("/Game/Maps/Level_1?listen"));
		World->ServerTravel(FString(ChosenLevelPath));
	}
}

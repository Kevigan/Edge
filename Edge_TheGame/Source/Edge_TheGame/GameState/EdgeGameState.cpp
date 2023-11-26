// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgeGameState.h"
#include "Net/UnrealNetwork.h"
#include "Edge_TheGame/PlayerState/EdgePlayerState.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Edge_TheGame/GameMode/EdgeGameMode.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"

void AEdgeGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEdgeGameState, TopScoringPlayers);
	DOREPLIFETIME(AEdgeGameState, RedTeamScore);
	DOREPLIFETIME(AEdgeGameState, BlueTeamScore);
}

void AEdgeGameState::UpdateTopScore(AEdgePlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void AEdgeGameState::RedTeamScores()
{
	++RedTeamScore;

	AEdgePlayerController* EdgePlayerController = Cast<AEdgePlayerController>(GetWorld()->GetFirstPlayerController());
	if (EdgePlayerController)
	{
		EdgePlayerController->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AEdgeGameState::BlueTeamScores()
{
	++BlueTeamScore;

	AEdgePlayerController* EdgePlayerController = Cast<AEdgePlayerController>(GetWorld()->GetFirstPlayerController());
	if (EdgePlayerController)
	{
		EdgePlayerController->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

void AEdgeGameState::OnRep_RedTeamScore()
{
	AEdgePlayerController* EdgePlayerController = Cast<AEdgePlayerController>(GetWorld()->GetFirstPlayerController());
	if (EdgePlayerController)
	{
		EdgePlayerController->SetHUDRedTeamScore(RedTeamScore);
	}
}

void AEdgeGameState::OnRep_BlueTeamScore()
{
	AEdgePlayerController* EdgePlayerController = Cast<AEdgePlayerController>(GetWorld()->GetFirstPlayerController());
	if (EdgePlayerController)
	{
		EdgePlayerController->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

void AEdgeGameState::MultiCastFinishGame_Implementation()
{
	EdgeGameMode = EdgeGameMode == nullptr ? GetWorld()->GetAuthGameMode<AEdgeGameMode>() : EdgeGameMode;

	if (EdgeGameMode)
	{
		EdgeGameMode->FinishGame();
	}
}

void AEdgeGameState::ServerFinishGame_Implementation()
{
	if (HasAuthority())
	{
		MultiCastFinishGame();
	}
	
}


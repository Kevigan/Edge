// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgeGameState.h"
#include "Net/UnrealNetwork.h"
#include "Edge_TheGame/PlayerState/EdgePlayerState.h"

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

void AEdgeGameState::OnRep_RedTeamScore()
{
}

void AEdgeGameState::OnRep_BlueTeamScore()
{
}

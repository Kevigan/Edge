// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgePlayerState.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Net/UnrealNetwork.h"

void AEdgePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEdgePlayerState, Death);
	DOREPLIFETIME(AEdgePlayerState, Team);
}

void AEdgePlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<AEdgeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AEdgePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDKills(GetScore());
		}
	}
}

void AEdgePlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<AEdgeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AEdgePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDKills(GetScore());
		}
	}
}

void AEdgePlayerState::AddToDeath(int32 DeathsAmount)
{
	Death += DeathsAmount;
	Character = Character == nullptr ? Cast<AEdgeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AEdgePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDeath(Death);
		}
	}
}

void AEdgePlayerState::SetTeam(ETeam TeamToSet)
{
	Team = TeamToSet;

	AEdgeCharacter* EdgeCharacter = Cast<AEdgeCharacter>(GetPawn());
	if (EdgeCharacter)
	{
		EdgeCharacter->SetTeamColor(Team);
	}
}

void AEdgePlayerState::OnRep_Team()
{
	AEdgeCharacter* EdgeCharacter = Cast<AEdgeCharacter>(GetPawn());
	if (EdgeCharacter)
	{
		EdgeCharacter->SetTeamColor(Team);
	}
}

void AEdgePlayerState::OnRep_Death()
{
	Character = Character == nullptr ? Cast<AEdgeCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AEdgePlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDeath(Death);
		}
	}
}





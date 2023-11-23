// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsGameMode.h"
#include "Edge_TheGame/GameState/EdgeGameState.h"
#include "Edge_TheGame/PlayerState/EdgePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"

ATeamsGameMode::ATeamsGameMode()
{
	bTeamsMatch = true;
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AEdgeGameState* EdgeGameState = Cast<AEdgeGameState>(UGameplayStatics::GetGameState(this));
	if (EdgeGameState)
	{
		AEdgePlayerState* EdgePlayerState = NewPlayer->GetPlayerState<AEdgePlayerState>();
		if (EdgePlayerState && EdgePlayerState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (EdgeGameState->BlueTeam.Num() >= EdgeGameState->RedTeam.Num())
			{
				EdgeGameState->RedTeam.AddUnique(EdgePlayerState);
				EdgePlayerState->SetTeam(ETeam::ET_RedTeam);
			}
			else
			{
				EdgeGameState->BlueTeam.AddUnique(EdgePlayerState);
				EdgePlayerState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	AEdgeGameState* EdgeGameState = Cast<AEdgeGameState>(UGameplayStatics::GetGameState(this));
	AEdgePlayerState* EdgePlayerState = Exiting->GetPlayerState<AEdgePlayerState>();
	if (EdgeGameState && EdgePlayerState)
	{
		if (EdgeGameState->RedTeam.Contains(EdgePlayerState))
		{
			EdgeGameState->RedTeam.Remove(EdgePlayerState);
		}
		if (EdgeGameState->BlueTeam.Contains(EdgePlayerState))
		{
			EdgeGameState->BlueTeam.Remove(EdgePlayerState);
		}
	}
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	AEdgeGameState* EdgeGameState = Cast<AEdgeGameState>(UGameplayStatics::GetGameState(this));
	if (EdgeGameState)
	{
		for (auto PlayerState : EdgeGameState->PlayerArray)
		{
			AEdgePlayerState* EdgePlayerState = Cast<AEdgePlayerState>(PlayerState.Get());
			if (EdgePlayerState && EdgePlayerState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (EdgeGameState->BlueTeam.Num() >= EdgeGameState->RedTeam.Num())
				{
					EdgeGameState->RedTeam.AddUnique(EdgePlayerState);
					EdgePlayerState->SetTeam(ETeam::ET_RedTeam);
				}
				else
				{
					EdgeGameState->BlueTeam.AddUnique(EdgePlayerState);
					EdgePlayerState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	AEdgePlayerState* AttackerPState = Attacker->GetPlayerState<AEdgePlayerState>();
	AEdgePlayerState* VictimPState = Victim->GetPlayerState<AEdgePlayerState>();
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString(TEXT("1111111")));

	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString(TEXT("2222222")));
	if (VictimPState == AttackerPState)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString(TEXT("33333333")));
		return 0.f;
	}
	if (AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString(TEXT("44444444")));
		return 0.f;
	}
	return BaseDamage;
}

void ATeamsGameMode::PlayerEliminated(AEdgeCharacter* ElimmedCharacter, AEdgePlayerController* VictimController, AEdgePlayerController* AttackerContoller)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerContoller);

	AEdgeGameState* EdgeGameState = Cast<AEdgeGameState>(UGameplayStatics::GetGameState(this));
	AEdgePlayerState* AttackerPlayerState = AttackerContoller ? Cast<AEdgePlayerState>(AttackerContoller->PlayerState) : nullptr;
	if (EdgeGameState && AttackerPlayerState)
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			EdgeGameState->BlueTeamScores();
		}
		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			EdgeGameState->RedTeamScores();
		}
	}
}

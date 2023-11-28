// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgeGameMode.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Edge_TheGame/PlayerState/EdgePlayerState.h"
#include "Edge_TheGame/GameState/EdgeGameState.h"

namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

AEdgeGameMode::AEdgeGameMode()
{
	bDelayedStart = true;
}



void AEdgeGameMode::BeginPlay()
{
	Super::BeginPlay();


	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void AEdgeGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountDownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			StartMatch();// sets MatchState to InProgress
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountDownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountDownTime <= 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountDownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("cooldownTime: %f"), CountDownTime));
		if (CountDownTime <= 0.f)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("restarting"));
			RestartGame();
		}
	}
	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("GameModeServerCountdown: %f"), CountDownTime));
	}
	
}

void AEdgeGameMode::FinishGame()
{
	SetMatchState(MatchState::Cooldown);
	MatchTime = 0.f;
	OnMatchStateSet();
}

void AEdgeGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AEdgePlayerController* EdgePlayerController = Cast<AEdgePlayerController>(*It);
		if (EdgePlayerController)
		{
			EdgePlayerController->OnMatchStateSet(MatchState, bTeamsMatch);
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString(TEXT("OnMatchStateSet")));
		}
	}
}

float AEdgeGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

void AEdgeGameMode::PlayerEliminated(AEdgeCharacter* ElimmedCharacter, AEdgePlayerController* VictimController, AEdgePlayerController* AttackerContoller)
{
	AEdgePlayerState* AttackerPlayerState = AttackerContoller ? Cast<AEdgePlayerState>(AttackerContoller->PlayerState) : nullptr;
	AEdgePlayerState* VictimPlayerState = VictimController ? Cast<AEdgePlayerState>(VictimController->PlayerState) : nullptr;

	AEdgeGameState* EdgeGameState = GetGameState<AEdgeGameState>();

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && EdgeGameState)
	{
		AttackerPlayerState->AddToScore(1.0f);
		EdgeGameState->UpdateTopScore(AttackerPlayerState);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDeath(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}
	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AEdgePlayerController* EdgePlayer = Cast<AEdgePlayerController>(*It);
		if (EdgePlayer && AttackerPlayerState && VictimPlayerState)
		{
			EdgePlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

void AEdgeGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ELimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if (ELimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ELimmedController, PlayerStarts[Selection]);
	}
}

void AEdgeGameMode::PlayerLeftGame(AEdgePlayerState* PlayerLeaving)
{
	if (PlayerLeaving == nullptr) return;
	AEdgeGameState* EdgeGameState = GetGameState<AEdgeGameState>();
	if (EdgeGameState && EdgeGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		EdgeGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}
	AEdgeCharacter* CharacterLeaving = Cast<AEdgeCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}




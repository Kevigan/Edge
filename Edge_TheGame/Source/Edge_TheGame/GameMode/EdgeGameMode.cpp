// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgeGameMode.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Edge_TheGame/PlayerState/EdgePlayerState.h"

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
			StartMatch();
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
}

void AEdgeGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AEdgePlayerController* EdgePlayer = Cast<AEdgePlayerController>(*It);
		if (EdgePlayer)
		{
			EdgePlayer->OnMatchStateSet(MatchState);
		}
	}
}

void AEdgeGameMode::PlayerEliminated(AEdgeCharacter* ElimmedCharacter, AEdgePlayerController* VictimController, AEdgePlayerController* AttackerContoller)
{
	AEdgePlayerState* AttackerPlayerState = AttackerContoller ? Cast<AEdgePlayerState>(AttackerContoller->PlayerState) : nullptr;
	AEdgePlayerState* VictimPlayerState = VictimController ? Cast<AEdgePlayerState>(VictimController->PlayerState) : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.0f);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDeath(1);
	}

	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
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



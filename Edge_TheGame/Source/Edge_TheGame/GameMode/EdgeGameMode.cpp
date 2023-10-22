// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgeGameMode.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Edge_TheGame/PlayerState/EdgePlayerState.h"

void AEdgeGameMode::PlayerEliminated(AEdgeCharacter* ElimmedCharacter, AEdgePlayerController* VictimController, AEdgePlayerController* AttackerContoller)
{
	AEdgePlayerState* AttackerPlayerState = AttackerContoller ? Cast<AEdgePlayerState>(AttackerContoller->PlayerState) :nullptr;
	AEdgePlayerState* VictimPlayerState = VictimController ? Cast<AEdgePlayerState>(VictimController->PlayerState) :nullptr;

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
		int32 Selection = FMath::RandRange(0,PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ELimmedController, PlayerStarts[Selection]);
	}
}

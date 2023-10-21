// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgeGameMode.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Edge_TheGame/PlayerController/EdgePlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void AEdgeGameMode::PlayerEliminated(AEdgeCharacter* ElimmedCharacter, AEdgePlayerController* VictimController, AEdgePlayerController* AttackerContoller)
{
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

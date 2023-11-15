// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "EdgeGameState.generated.h"

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API AEdgeGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(class AEdgePlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
		TArray<AEdgePlayerState*> TopScoringPlayers;


	/*
	* Teams
	*/
	void RedTeamScores();
	void BlueTeamScores();

	UPROPERTY(BlueprintReadWrite)
		TArray<AEdgePlayerState*> RedTeam;
	UPROPERTY(BlueprintReadWrite)
		TArray<AEdgePlayerState*> BlueTeam;

	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
		float RedTeamScore = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
		float BlueTeamScore = 0.f;

	UFUNCTION()
		void OnRep_RedTeamScore();

	UFUNCTION()
		void OnRep_BlueTeamScore();

	UFUNCTION(BlueprintCallable)
		void TestShit();

	UFUNCTION(Server, Reliable)
		void ServerDoSOmething();

	UFUNCTION(NetMulticast, Reliable)
		void MulticastDoSOmething(const TArray<AEdgePlayerState*>& Team);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "TestingShit"))
		void ReceiveOnShit(/*UPARAM(ref) TArray<AEdgePlayerState*>& Shit*/);

protected:

private:
	float TopScore = 0.f;


};

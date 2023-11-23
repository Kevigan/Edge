// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Edge_TheGame/EdgeTypes/Team.h"
#include "EdgePlayerState.generated.h"

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API AEdgePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	UFUNCTION()
		virtual void OnRep_Death();

	void AddToScore(float ScoreAmount);
	void AddToDeath(int32 DeathsAmount);

protected:

private:
	UPROPERTY()
		class AEdgeCharacter* Character;
	UPROPERTY()
		class AEdgePlayerController* Controller;

	UPROPERTY(ReplicatedUsing = OnRep_Death)
		int32 Death;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
		ETeam Team = ETeam::ET_NoTeam;

	UFUNCTION()
		void OnRep_Team();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam TeamToSet);

	UFUNCTION(BlueprintCallable)
		int32 GetDeath() { return Death; }

};

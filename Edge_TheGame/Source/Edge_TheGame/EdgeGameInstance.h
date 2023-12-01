// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EdgeGameInstance.generated.h"

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API UEdgeGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FString CurrentPistolString;

	UPROPERTY(BlueprintReadWrite)
		FString CurrentShotgunString;

	UPROPERTY(BlueprintReadWrite)
		FString CurrentRifleString;

	UPROPERTY(BlueprintReadWrite)
		FString CurrentSniperString;

	UPROPERTY(BlueprintReadWrite)
		FString CurrentCharacterString;
};

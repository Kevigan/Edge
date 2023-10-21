// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EdgePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class EDGE_THEGAME_API AEdgePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetHUDHealth(float Health, float MaxHealth);

protected:
	virtual void BeginPlay() override;
	
private:
	class AEdge_HUD* EdgeHUD;

};

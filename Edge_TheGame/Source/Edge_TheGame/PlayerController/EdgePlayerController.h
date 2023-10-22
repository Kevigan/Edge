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
	void SetHUDKills(float Score);
	void SetHUDDeath(int32 Value);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedWeaponAmmo(int32 Ammo);
	void OnPossess(APawn* InPawn) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
		class AEdge_HUD* EdgeHUD;

};

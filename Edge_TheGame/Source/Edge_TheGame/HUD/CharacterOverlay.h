// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterOverlay.generated.h"

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthBar = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* HealthText = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* KillAmount = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* DeathAmount = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* WeaponAmmoAmount = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* CarriedWeaponAmmoAmount = nullptr;

	UPROPERTY(meta = (BindWidget))
		UTextBlock* MatchCountdownText = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UImage* HighPingImage = nullptr;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
		UWidgetAnimation* HighPingAnimation = nullptr;
		
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* RedTeamScore = nullptr;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* BlueTeamScore = nullptr;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMenu.generated.h"

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API UGameMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	void MenuSetup();
	void MenuTearDown();

protected:
	virtual bool Initialize() override;

	UFUNCTION()
		void OnDestroySession(bool bWasSuccessful);

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* ReturnToMainMenuButton;

	UFUNCTION()
		void ReturnToMainMenuButtonClicked();

	UPROPERTY()
		class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
		class APlayerController* PlayerController;
};

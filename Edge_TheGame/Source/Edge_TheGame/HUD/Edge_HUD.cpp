#include "Edge_HUD.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "Edge_HUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.h"
#include "Components/TextBlock.h"
#include "TeamDataWidget.h"



void AEdge_HUD::BeginPlay()
{
	Super::BeginPlay();

}

void AEdge_HUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void AEdge_HUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass)
	{
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

void AEdge_HUD::AddMiniMap()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && MiniMapOverlayClass)
	{
		MiniMapOverlay = CreateWidget<UUserWidget>(PlayerController, MiniMapOverlayClass);
		MiniMapOverlay->AddToViewport();
	}
}

void AEdge_HUD::AddKillText()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && KillTextOverlayClass)
	{
		KillTextOverlay = CreateWidget<UUserWidget>(PlayerController, KillTextOverlayClass);
		KillTextOverlay->AddToViewport();
	}
}

void AEdge_HUD::SetEnemyKilledText(const FString& EnemyName)
{
	if (CharacterOverlay)
	{
		FString NewStringText =  "You destroyed " + EnemyName + "!";
		CharacterOverlay->EnemyKilledText->SetText(FText::FromString(NewStringText));
		CharacterOverlay->ReceiveOnSetEnemyKilledText(EnemyName);
	}
}

void AEdge_HUD::AddTeamDataWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && TeamDataWidgetClass)
	{
		TeamDataWidgetOverlay = CreateWidget<UTeamDataWidget>(PlayerController, TeamDataWidgetClass);
		TeamDataWidgetOverlay->AddToViewport();
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetIgnoreLookInput(true);
	}
}

void AEdge_HUD::RemoveTeamDataWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && TeamDataWidgetOverlay)
	{
		TeamDataWidgetOverlay->RemoveFromParent();
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetIgnoreLookInput(false);
	}
}

void AEdge_HUD::DrawHUD()
{
	Super::DrawHUD();
	FVector2D ViewportSize;
	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;

		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
	}
}

void AEdge_HUD::DrawCrosshair(UTexture2D* Texture, FVector2D VierwportCenter, FVector2D Spread, FLinearColor CrosshairsColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		VierwportCenter.X - (TextureWidth / 2.f) + Spread.X,
		VierwportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairsColor
	);
}


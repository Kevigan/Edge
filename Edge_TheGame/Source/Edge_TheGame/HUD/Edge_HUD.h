// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Edge_HUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()
public:
	UPROPERTY()
		class UTexture2D* CrosshairsCenter;
	UPROPERTY()
		UTexture2D* CrosshairsLeft;
	UPROPERTY()
		UTexture2D* CrosshairsRight;
	UPROPERTY()
		UTexture2D* CrosshairsTop;
	UPROPERTY()
		UTexture2D* CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor;
};

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API AEdge_HUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = Config = PlayerStats)
		TSubclassOf<class UCharacterOverlay> CharacterOverlayClass;

	UPROPERTY(EditAnywhere, Category = Config = PlayerStats)
		TSubclassOf<class UUserWidget> MiniMapOverlayClass;

	UPROPERTY(EditAnywhere, Category = Config = PlayerStats)
		TSubclassOf< UUserWidget> AnnouncementClass;

	UPROPERTY(EditAnywhere, Category = Config = PlayerStats)
		TSubclassOf<class UMenuWidget> MenuWidgetClass;

	UPROPERTY(EditAnywhere, Category = Config = PlayerStats)
		TSubclassOf<class UTeamDataWidget> TeamDataWidgetClass;

	UPROPERTY(EditAnywhere, Category = Config = PlayerStats)
		TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass;

	UPROPERTY()
		UCharacterOverlay* CharacterOverlay = nullptr;

	UPROPERTY()
		UUserWidget* MiniMapOverlay = nullptr;

	UPROPERTY()
		UMenuWidget* MenuWidgetOverlay = nullptr;

	UPROPERTY()
		UTeamDataWidget* TeamDataWidgetOverlay = nullptr;

	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void ReceiveOnShowHitUI();

	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void ReceiveOnShowHitNumbers(AEdgeCharacter* DamagedCharacter, float Damage);

	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
		void ReceiveOnShowDeathUI(AEdgeCharacter* DamagedCharacter);

	void AddCharacterOverlay();

	UPROPERTY()
		class UAnnouncement* Announcement = nullptr;

	void InitAnnouncement();
	void AddAnnouncement();
	void AddElimAnnouncement(FString Attacker, FString Victim, class APlayerState* AttackerPS, APlayerState* VictimPS);
	void AddMiniMap();
	
	void AddTeamDataWidget();
	void RemoveTeamDataWidget();

protected:
	virtual void BeginPlay() override;

private:
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D VierwportCenter, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere)
		float CrosshairSpreadMax = 16.f;

	UPROPERTY()
		APlayerController* OwningPlayer;

	UPROPERTY(EditAnywhere, Category = Config)
		float ElimAnnouncementTime = 2.5f;

	UFUNCTION()
	void ELimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove);

	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages;

public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }

	UFUNCTION(BlueprintCallable)
		UCharacterOverlay* GetCharacterOverlay(){return CharacterOverlay;}
};

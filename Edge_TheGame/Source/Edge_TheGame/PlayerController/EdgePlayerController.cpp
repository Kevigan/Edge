// Fill out your copyright notice in the Description page of Project Settings.


#include "EdgePlayerController.h"
#include "Edge_TheGame/HUD/Edge_HUD.h"
#include "Edge_TheGame/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Edge_TheGame/Character/EdgeCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Edge_TheGame/GameMode/EdgeGameMode.h"
#include "Edge_TheGame/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"

void AEdgePlayerController::BeginPlay()
{
	Super::BeginPlay();

	EdgeHUD = Cast<AEdge_HUD>(GetHUD());
	ServerCheckMatchState();
}

void AEdgePlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEdgePlayerController, MatchState);
}

void AEdgePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
}

void AEdgePlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void AEdgePlayerController::ServerCheckMatchState_Implementation()
{
	AEdgeGameMode* GameMode = Cast<AEdgeGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		levelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, levelStartingTime);
	}
}

void AEdgePlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	levelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);

	if (EdgeHUD && MatchState == MatchState::WaitingToStart)
	{
		EdgeHUD->AddAnnouncement();
	}
}

void AEdgePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	AEdgeCharacter* EdgeCharacter = Cast<AEdgeCharacter>(InPawn);
	if (EdgeCharacter)
	{
		SetHUDHealth(EdgeCharacter->GetHealth(), EdgeCharacter->GetMaxHealth());
	}
}

void AEdgePlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;

	bool bHUDValid = EdgeHUD && EdgeHUD->CharacterOverlay && EdgeHUD->CharacterOverlay->HealthBar && EdgeHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercent = Health / MaxHealth;
		EdgeHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		EdgeHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void AEdgePlayerController::SetHUDKills(float Score)
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	bool bHUDValid = EdgeHUD && EdgeHUD->CharacterOverlay && EdgeHUD->CharacterOverlay->HealthBar && EdgeHUD->CharacterOverlay->KillAmount;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		EdgeHUD->CharacterOverlay->KillAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDKills = Score;
	}

}

void AEdgePlayerController::SetHUDDeath(int32 Value)
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	bool bHUDValid = EdgeHUD && EdgeHUD->CharacterOverlay && EdgeHUD->CharacterOverlay->HealthBar && EdgeHUD->CharacterOverlay->DeathAmount;
	if (bHUDValid)
	{
		FString DeathText = FString::Printf(TEXT("%d"), Value);
		EdgeHUD->CharacterOverlay->DeathAmount->SetText(FText::FromString(DeathText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDeaths = Value;
	}
}

void AEdgePlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	bool bHUDValid = EdgeHUD && EdgeHUD->CharacterOverlay && EdgeHUD->CharacterOverlay->HealthBar && EdgeHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		EdgeHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AEdgePlayerController::SetHUDCarriedWeaponAmmo(int32 Ammo)
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	bool bHUDValid = EdgeHUD && EdgeHUD->CharacterOverlay && EdgeHUD->CharacterOverlay->HealthBar && EdgeHUD->CharacterOverlay->CarriedWeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		EdgeHUD->CharacterOverlay->CarriedWeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void AEdgePlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	bool bHUDValid = EdgeHUD && EdgeHUD->CharacterOverlay && EdgeHUD->CharacterOverlay->HealthBar && EdgeHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		EdgeHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void AEdgePlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	bool bHUDValid = EdgeHUD && EdgeHUD->Announcement && EdgeHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		EdgeHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AEdgePlayerController::SetHUDTime()
{
	float TimeLeft =  0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + levelStartingTime;
	else if(MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + levelStartingTime;
		
	

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;
}

void AEdgePlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (EdgeHUD && EdgeHUD->CharacterOverlay)
		{
			CharacterOverlay = EdgeHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDKills(HUDKills);
				SetHUDDeath(HUDDeaths);
			}
		}
	}
}

void AEdgePlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void AEdgePlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float AEdgePlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void AEdgePlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void AEdgePlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AEdgePlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void AEdgePlayerController::HandleMatchHasStarted()
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	if (EdgeHUD)
	{
		EdgeHUD->AddCharacterOverlay();
		if (EdgeHUD->Announcement)
		{
			EdgeHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AEdgePlayerController::HandleCooldown()
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	if (EdgeHUD)
	{
		EdgeHUD->CharacterOverlay->RemoveFromParent();
		if (EdgeHUD->Announcement)
		{
			EdgeHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

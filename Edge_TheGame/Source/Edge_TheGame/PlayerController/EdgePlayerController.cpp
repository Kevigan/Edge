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
#include "Edge_TheGame/EdgeComponents/CombatComponent.h"
#include "Edge_TheGame/GameState/EdgeGameState.h"
#include "Edge_TheGame/PlayerState/EdgePlayerState.h"
#include "Components/Image.h"
#include "Edge_TheGame/HUD/GameMenu.h"

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

void AEdgePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;

	InputComponent->BindAction("ESC", IE_Pressed, this, &ThisClass::ShowGameMenu);
}

void AEdgePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
	CheckPing(DeltaTime);

}

void AEdgePlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			UE_LOG(LogTemp, Warning, TEXT("PlayerState->GetPin() * 4: %d"), PlayerState->GetPing() * 4);
			if (PlayerState->GetPing() * 4 > HighPingThreshold) // ping is ccompressed; it´s actually ping / 4
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.f;
	}
	bool bHidePingAnimationPlaying = EdgeHUD && EdgeHUD->CharacterOverlay &&
		EdgeHUD->CharacterOverlay->HighPingAnimation &&
		EdgeHUD->CharacterOverlay->IsAnimationPlaying(EdgeHUD->CharacterOverlay->HighPingAnimation);
	if (bHidePingAnimationPlaying)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

void AEdgePlayerController::ShowGameMenu()
{
	if (GameMenuWidget == nullptr) return;
	if (GameMenu == nullptr)
	{
		GameMenu = CreateWidget<UGameMenu>(this, GameMenuWidget);
	}
	if (GameMenu)
	{
		bGameMenuOpen = !bGameMenuOpen;
		if (bGameMenuOpen)
		{
			GameMenu->MenuSetup();
		}
		else
		{
			GameMenu->MenuTearDown();
		}
	}

}

//Is the ping too high?
void AEdgePlayerController::ServerReportPingStatus_Implementation(bool bHidePing)
{
	HighPingDelegate.Broadcast(bHidePing);
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

void AEdgePlayerController::HighPingWarning()
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;

	bool bHUDValid = EdgeHUD && EdgeHUD->CharacterOverlay && EdgeHUD->CharacterOverlay->HighPingImage && EdgeHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		EdgeHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		EdgeHUD->CharacterOverlay->PlayAnimation(EdgeHUD->CharacterOverlay->HighPingAnimation, 0.f, 5);
	}
}

void AEdgePlayerController::StopHighPingWarning()
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;

	bool bHUDValid = EdgeHUD && EdgeHUD->CharacterOverlay && EdgeHUD->CharacterOverlay->HighPingImage && EdgeHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		EdgeHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
		if (EdgeHUD->CharacterOverlay->IsAnimationPlaying(EdgeHUD->CharacterOverlay->HighPingAnimation))
		{
			EdgeHUD->CharacterOverlay->StopAnimation(EdgeHUD->CharacterOverlay->HighPingAnimation);
		}
	}
}

void AEdgePlayerController::ServerCheckMatchState_Implementation()
{
	AEdgeGameMode* GameMode = Cast<AEdgeGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		levelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, levelStartingTime, CooldownTime);
	}
}

void AEdgePlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
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
		bInitializeHUDHealth = true;
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
		bInitializeHUDKills = true;
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
		bInitializeHUDDeaths = true;
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
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
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
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

void AEdgePlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	bool bHUDValid = EdgeHUD && EdgeHUD->CharacterOverlay && EdgeHUD->CharacterOverlay->HealthBar && EdgeHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			EdgeHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
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
		if (CountdownTime < 0.f)
		{
			EdgeHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		EdgeHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void AEdgePlayerController::SetHUDTime()
{
	if (HasAuthority())
	{
		EdgeGameMode = EdgeGameMode == nullptr ? Cast<AEdgeGameMode>(UGameplayStatics::GetGameMode(this)) : EdgeGameMode;
		if (EdgeGameMode)
		{
			//SecondsLeft = FMath::CeilToInt(EdgeGameMode->GetCountdownTime()/* + EdgeGameMode->LevelStartingTime*/);
			levelStartingTime = EdgeGameMode->LevelStartingTime;
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("gameModeStartingTime: %f"), EdgeGameMode->LevelStartingTime));
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("ServerlevelStartingTime: %f"), levelStartingTime));
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("MatchState: %s"), *MatchState.ToString()));
		}
	}
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + levelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + levelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + levelStartingTime;

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	GetWorld()->GetTimeSeconds();

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
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
				//GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Red, FString::Printf(TEXT("healt: %f"), HUDHealth));
				if (bInitializeHUDHealth)SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeHUDKills)SetHUDKills(HUDKills);
				if (bInitializeHUDDeaths)SetHUDDeath(HUDDeaths);
				if (bInitializeCarriedAmmo) SetHUDCarriedWeaponAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
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
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
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
		if (EdgeHUD->CharacterOverlay == nullptr) EdgeHUD->AddCharacterOverlay();//fix for doubling Character Overlay

		//EdgeHUD->AddCharacterOverlay();

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
		bool bHUDValid = EdgeHUD->Announcement && EdgeHUD->Announcement->AnnouncementText && EdgeHUD->Announcement->InfoText;
		if (bHUDValid)
		{
			EdgeHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText("New Match Starts In:");
			EdgeHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			AEdgeGameState* EdgeGameState = Cast<AEdgeGameState>(UGameplayStatics::GetGameState(this));
			AEdgePlayerState* EdgePlayerState = GetPlayerState<AEdgePlayerState>();
			if (EdgeGameState && EdgePlayerState)
			{
				TArray<AEdgePlayerState*> TopPlayers = EdgeGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("There is no winner");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == EdgePlayerState)
				{
					InfoTextString = FString("You are the winner!");
				}
				else if (TopPlayers.Num() == 1)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win:\n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				EdgeHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}


		}
	}
	AEdgeCharacter* EdgeCharacter = Cast<AEdgeCharacter>(GetPawn());
	if (EdgeCharacter && EdgeCharacter->GetCombat())
	{
		EdgeCharacter->bDisableGameplay = true;
		EdgeCharacter->GetCombat()->FireButtonPressed(false);
	}
}

void AEdgePlayerController::ShowTeamData()
{
	EdgeHUD = EdgeHUD == nullptr ? Cast<AEdge_HUD>(GetHUD()) : EdgeHUD;
	if (EdgeHUD && !TeamDataOpen)
	{
		EdgeHUD->AddTeamDataWidget();
		TeamDataOpen = true;
	}
	else if (EdgeHUD && TeamDataOpen)
	{
		EdgeHUD->RemoveTeamDataWidget();
		TeamDataOpen = false;
	}
}

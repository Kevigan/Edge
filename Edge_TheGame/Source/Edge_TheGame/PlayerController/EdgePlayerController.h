// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EdgePlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

/**
 *
 */
UCLASS()
class EDGE_THEGAME_API AEdgePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDKills(float Score);
	void SetHUDDeath(int32 Value);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedWeaponAmmo(int32 Ammo);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	void OnPossess(APawn* InPawn) override;

	virtual float GetServerTime(); // synced with server world clock
	virtual void ReceivedPlayer() override; // sync with server clock as soon as possible

	void OnMatchStateSet(FName State, bool bTeamsMatch = false);
	void HandleMatchHasStarted(bool bTeamsMatch = false);
	void HandleCooldown();
	void ShowTeamData();
	void HideTeamData();
	bool MenuOpen = false;
	bool TeamDataOpen = false;

	float SingleTripTime = 0.f;

	FHighPingDelegate HighPingDelegate;

	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

	void HideTeamScores();
	void InitTeamScores();
	void SetHUDRedTeamScore(int32 RedScore);
	void SetHUDBlueTeamScore(int32 BlueScore);

	UFUNCTION(BlueprintImplementableEvent)
		void OnTeamWin();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

	void SetHUDTime();
	void PollInit();

	UFUNCTION(Server, Reliable)
		void ServerFinishGame();

	UFUNCTION(NetMulticast, Reliable)
		void MultiCastFinishGame();

	/// <summary>
	/// Sync time between client and server
	/// </summary>

	//Requests the current server time passing in the clients time when the request was sent
	UFUNCTION(Server, Reliable)
		void ServerRequestServerTime(float TimeOfClientRequest);

	//Reports the current server time to the client in response to ServerRequestServerTime
	UFUNCTION(Client, Reliable)
		void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta = 0.f; // difference between client and server time

	UPROPERTY(EditAnywhere, Category = Config = Time)
		float TimeSyncFrequency = 5.f;

	float TimeSyncRunningTime = 0.f;
	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
		void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
		void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Cooldown);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);

	void ShowGameMenu();

	UFUNCTION(Client, Reliable)
		void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamsScores)
	bool bShowTeamScores = false;

	UFUNCTION()
	void OnRep_ShowTeamsScores();

	UPROPERTY(EditAnywhere, Category = Config)
		int32 NumberOfKillsNeededToWin = 1;

private:
	UPROPERTY()
		class AEdge_HUD* EdgeHUD = nullptr;

	UPROPERTY()
		class AEdgeGameMode* EdgeGameMode = nullptr;

	/*
	* Open Game Menu
	*/
	UPROPERTY(EditAnywhere, Category = Config = HUD)
		TSubclassOf<class UUserWidget> GameMenuWidget;

	UPROPERTY()
		class UGameMenu* GameMenu;

	bool bGameMenuOpen = false;


	float levelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
		FName MatchState;

	UFUNCTION()
		void OnRep_MatchState();

	UPROPERTY()
		class UCharacterOverlay* CharacterOverlay = nullptr;
	bool bInitializeCharacterOverlay = false;

	bool bInitializeHUDHealth = false;
	float HUDHealth;

	float HUDMaxHealth;

	bool bInitializeHUDKills = false;
	float HUDKills;

	bool bInitializeHUDDeaths = false;
	int32 HUDDeaths;

	bool bInitializeCarriedAmmo = false;
	float HUDCarriedAmmo;

	bool bInitializeWeaponAmmo = false;
	float HUDWeaponAmmo;

	float HighPingRunningTime = 0.f;

	UPROPERTY(EditAnywhere, Category = Config)
		float HighPingDuration = 5.f;

	float PingAnimationRunningTime = 0.f;

	UPROPERTY(EditAnywhere, Category = Config)
		float CheckPingFrequency = 20.f;

	UFUNCTION(Server, Reliable)
		void ServerReportPingStatus(bool bHidePing);

	UPROPERTY(EditAnywhere, Category = Config)
		float HighPingThreshold = 50.f;

};

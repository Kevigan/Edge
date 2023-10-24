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

	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
	void HandleCooldown();


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetHUDTime();
	void PollInit();

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
private:
	UPROPERTY()
		class AEdge_HUD* EdgeHUD;

	UPROPERTY()
		class AEdgeGameMode* EdgeGameMode;

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
		class UCharacterOverlay* CharacterOverlay;
	bool bInitializeCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDKills;
	int32 HUDDeaths;
};

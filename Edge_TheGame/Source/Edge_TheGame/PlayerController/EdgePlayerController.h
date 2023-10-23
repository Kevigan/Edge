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
	void SetHUDMatchCountdown(float CountdownTime);
	void OnPossess(APawn* InPawn) override;

	virtual float GetServerTime(); // synced with server world clock
	virtual void ReceivedPlayer() override; // sync with server clock as soon as possible

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SetHUDTime();

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

private:
	UPROPERTY()
		class AEdge_HUD* EdgeHUD;

	float MatchTime = 120.f;
	uint32 CountdownInt = 0;
};

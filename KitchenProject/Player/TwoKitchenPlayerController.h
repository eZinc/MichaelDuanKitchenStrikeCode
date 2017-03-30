// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "../Kitchen/KitchenManager.h"
#include "UnrealNetwork.h"
#include "TwoKitchenPlayerController.generated.h"

/**
*
*/
UCLASS()
class KITCHENPROJECT_API ATwoKitchenPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	class AComputer* ComputerInUse;

	UFUNCTION(BlueprintCallable, Category = Computer)
	void RequestComputerLogin(const FString& password);

	UFUNCTION(BlueprintCallable, Category = Computer)
	void RequestChangePassword(const FString& password);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestChangePassword(const FString& password);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestComputerLogin(const FString& password);

	UFUNCTION(Client, Reliable)
	void AllowComputerLogin(bool ShouldUnlock);

	UFUNCTION(BlueprintImplementableEvent, Category = Computer)
	void ComputerUnlock(bool ShouldUnlock);

	UFUNCTION(exec)
	void ready();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestReady();

	UFUNCTION(exec)
	void shopname(FString YourShopName);

	UFUNCTION(exec)
	void name(FString YourPlayerName);

	void PawnLeavingGame() override;

	UFUNCTION(BlueprintCallable, Category = Computer)
	void OrderItem(FName Name, UClass* ItemClass, int32 quantity, int32 price, int32 MinutesToDeliver);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerOrderItem(FName Name, UClass* ItemClass, int32 quantity, int32 price, int32 MinutesToDeliver);

	void BeginPlay() override;

	void DrawClientHUD(APawn* InPawn);

	UFUNCTION(Client, Reliable)
	void UpdateWallet(int32 amount, int32 total, const FString& Reason);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateWalletGUI(int32 amount, int32 total, const FString& Reason);

	UFUNCTION(BlueprintCallable, Category = Chat)
	void GiveMessage(FString ChatMessage);

	UFUNCTION(BlueprintImplementableEvent, Category = Chat)
	void TakeMessage(const FString& PlayerName, const FString& PlayerTeam, const FString& ChatMessage);

	UFUNCTION(BlueprintImplementableEvent, Category = HUD)
	void DrawSpectatorHUD();

	UFUNCTION(BlueprintImplementableEvent, Category = HUD)
	void DrawPlayerHUD();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdateShopName(const FString& ShopName);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdatePlayerName(const FString& PlayerName);

	UFUNCTION(Server, Reliable, WithValidation)
	void SendServerChatMessage(const FString& ChatMessage);

	UFUNCTION(Client, Reliable)
	void ServerRefreshScoreboard();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void ClientRefreshScoreboard();

	UFUNCTION(BlueprintCallable, Category = Pregame)
	void RequestTeam(int32 id);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestTeam(int32 id);

	UFUNCTION(Client, Reliable)
	void AllowedUseComputer();

	UFUNCTION(Client, Reliable)
	void ServerDrawPregameHUD();

	UFUNCTION(BlueprintImplementableEvent)
	void ClientDrawComputerHUD();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void ClientDrawPregameHUD();

	UFUNCTION(Client, Reliable)
	void ServerDrawSpectatorHUD();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void ClientDrawSpectatorHUD();

	UFUNCTION(Client, Reliable)
	void ServerDrawPlayerHUD();

	UFUNCTION(BlueprintImplementableEvent, Category = "HUD")
	void ClientDrawPlayerHUD();

	UFUNCTION(Client, Reliable)
	void ServerNotifyGameStarted();

	UFUNCTION(Client, Reliable)
	void ServerNotifyGameEnded(const FString& Winner, int32 WinnerTeam, int32 PlayerOneMoney, int32 PlayerTwoMoney, int32 PlayerOneRent, int32 PlayerTwoRent,
		int32 PlayerOneTotal, int32 PlayerTwoTotal, int32 PlayerOneOldStrikes, int32 PlayerTwoOldStrikes,
		int32 PlayerOneUpdatedStrikes, int32 PlayerTwoUpdatedStrikes);

	UFUNCTION(Client, Reliable)
	void ServerNotifyDayEnded(int32 PlayerOneMoney, int32 PlayerTwoMoney, int32 PlayerOneRent, int32 PlayerTwoRent, 
		int32 PlayerOneTotal, int32 PlayerTwoTotal, int32 PlayerOneOldStrikes, int32 PlayerTwoOldStrikes, 
		int32 PlayerOneUpdatedStrikes, int32 PlayerTwoUpdatedStrikes);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gamemode")
	void ClientNotifyGameStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Gamemode")
	void ClientNotifyGameEnded(const FString& Winner, int32 WinnerTeam, int32 PlayerOneMoney, int32 PlayerTwoMoney, int32 PlayerOneRent, int32 PlayerTwoRent,
		int32 PlayerOneTotal, int32 PlayerTwoTotal, int32 PlayerOneOldStrikes, int32 PlayerTwoOldStrikes,
		int32 PlayerOneUpdatedStrikes, int32 PlayerTwoUpdatedStrikes);

	UFUNCTION(BlueprintImplementableEvent, Category = "Gamemode")
	void ClientNotifyDayEnded(int32 PlayerOneMoney, int32 PlayerTwoMoney, int32 PlayerOneRent, int32 PlayerTwoRent,
		int32 PlayerOneTotal, int32 PlayerTwoTotal, int32 PlayerOneOldStrikes, int32 PlayerTwoOldStrikes,
		int32 PlayerOneUpdatedStrikes, int32 PlayerTwoUpdatedStrikes);

	UFUNCTION(Client, Reliable)
	void SendAllClientsMessage(const FString& PlayerName, const FString& PlayerTeam, const FString& ChatMessage);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerExitedComputer();

	UFUNCTION(BlueprintCallable, Category = Computer)
	void ClientExitedComputer();

	UFUNCTION(BlueprintImplementableEvent)
	void ExitComputerEffects();
};

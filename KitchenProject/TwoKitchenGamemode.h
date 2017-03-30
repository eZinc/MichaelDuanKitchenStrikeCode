// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "TwoKitchenGamemode.generated.h"

UCLASS(minimalapi)
class ATwoKitchenGamemode : public AGameMode
{
	GENERATED_BODY()

public:
	ATwoKitchenGamemode();

	void Tick(float DeltaTime) override;

	// Strike limit
	// TODO: Change GUI so that 0/3 isn't hardcoded
	UPROPERTY(EditDefaultsOnly, Category = GameSettings)
	int32 StrikeLimit;

	TArray<class ASimpleCarSpawner*> CarSpawners;

	// Minutes per day/round
	UPROPERTY(EditDefaultsOnly, Category = GameSettings)
	float MinutesPerDay;

	// Default starting rent
	UPROPERTY(EditDefaultsOnly, Category = GameSettings)
	int32 DefaultRentRate;

	UPROPERTY(EditDefaultsOnly, Category = GameSettings)
	int32 RentIncreasePerDay;

	// The accurate minutes that only the server has
	// This is to save packets of data
	float AccurateMinutes = 0;
	
	// These are character blueprints for each team
	UPROPERTY(EditDefaultsOnly, Category = Spawn)
	class UClass* PlayerOneBlueprint;
	UPROPERTY(EditDefaultsOnly, Category = Spawn)
	class UClass* PlayerTwoBlueprint;
	UPROPERTY(EditDefaultsOnly, Category = Spawn)
	class UClass* SpectatorBlueprint;

	// Spawn points for teams
	class ATargetPoint* PlayerOneSpawnPoint;
	class ATargetPoint* PlayerTwoSpawnPoint;
	class ATargetPoint* SpectatorSpawnPoint;

	// DEPRECATED
	int32 AssignOpenSlot(class ATwoKitchenPlayerController* p);

	// DEPRECATED...?
	UFUNCTION(NetMulticast, Reliable)
	void SpawnHUD(class ATwoKitchenPlayerController* controller, class APawn* InPawn);

	// Sends a message in the chat to everyone on the server
	UFUNCTION(BlueprintCallable, Category = Server)
	void SendConsoleMessage(FString message);

	void BeginPlay() override;

	// Starts the game and begins day/night cycle
	void StartGame();

	// Ends the game. If i is -1 it will perform checks for strikes
	// else it will treat the game as forfeited, giving the winner to not team i
	void EndGame(int32 i);

	// Called when a player chooses a team in the pre-game GUI
	// the controller is the player who called it
	// int32 team is the team the player chose
	void JoinTeam(class ATwoKitchenPlayerController* p, int32 team);

	// Tells players to refresh the entire scoreboard
	// TODO: Optimize this so that it doesn't refresh the entire scoreboard and tracks changes instead
	void TellClientsRefreshScoreboard();

	// Called when a player requests to be ready -- check if the game should start
	void PlayerReady(class ATwoKitchenPlayerController* p);

	void RespawnPlayer(class ATwoKitchenPlayerController* p);

private:
	// Called when a day has passed
	void IncrementDay();

	// Used for UI stuff -- ignore
	int32 PlayerOneOldStrikes;
	int32 PlayerTwoOldStrikes;

	// Collect rent from a kitchen and determine if the game is ending
	void CollectRent(class AKitchenManager* Kitchen);

	// DEPRECATED
	int32 cheapCount = 0;
	// DEPRECATED
	int32 RemoveFromSlot(class ATwoKitchenPlayerController* p);

	// Create a player given a player controller with a set player state already
	void CreatePlayer(class ATwoKitchenPlayerController* p);

	// Called when player leaves the game
	void PlayerLeft(class APlayerState* p);

	// Check if given array of player states are ready
	bool CheckIfTeamReady(TArray<class ATwoKitchenPlayerState*> a);

protected:
	void PostLogin(APlayerController* NewPlayer) override;
	void Logout(AController* Exiting);
};




// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "../Kitchen/KitchenManager.h"
#include "TwoKitchenPlayerController.h"
#include "TwoKitchenPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class KITCHENPROJECT_API ATwoKitchenPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	// for server use only
	ATwoKitchenPlayerController* PlayerController;

	UPROPERTY(Replicated)
	int32 TeamID = -1;

	UPROPERTY(Replicated)
	FString DisplayName = "NewPlayer";

	UPROPERTY(Replicated)
	int32 Points = 0;

	UFUNCTION(BlueprintCallable, Category = PlayerStats)
	int32 GetPoints() 
	{
		return Points;
	}

	UPROPERTY(Replicated)
	int32 Kills = 0;

	UFUNCTION(BlueprintCallable, Category = PlayerStats)
	int32 GetKills()
	{
		return Kills;
	}

	UPROPERTY(Replicated)
	int32 Deaths = 0;

	UFUNCTION(BlueprintCallable, Category = PlayerStats)
	int32 GetDeaths()
	{
		return Deaths;
	}

	void SetDeaths(int32 in)
	{
		Deaths = in;
	}

	void SetKills(int32 in)
	{
		Kills = in;
	}

	UPROPERTY(Replicated)
	int32 Assists = 0;

	UFUNCTION(BlueprintCallable, Category = PlayerStats)
	int32 GetAssists()
	{
		return Assists;
	}

	UPROPERTY(Replicated)
	int32 Mobscore = 0;

	UFUNCTION(BlueprintCallable, Category = PlayerStats)
	int32 GetMobscore()
	{
		return Mobscore;
	}

	UPROPERTY(Replicated)
	bool IsReady = false;

	UFUNCTION(BlueprintCallable, Category = PlayerStats)
	bool GetIsReady()
	{
		return IsReady;
	}

	void SetTeamID(int32 i)
	{
		TeamID = i;
	}

	UFUNCTION(BlueprintCallable, Category = Stats)
	int32 GetTeamID()
	{
		return TeamID;
	}

	UPROPERTY(Replicated, BlueprintReadOnly)
	class AKitchenManager* KitchenManager;

	void SetKitchenManager(class AKitchenManager* in)
	{
		KitchenManager = in;
	}

	class AKitchenManager* GetKitchenManager()
	{
		return KitchenManager;
	}

	void SetDisplayName(FString name)
	{
		DisplayName = name;
	}

	UFUNCTION(BlueprintCallable, Category = Scoreboard)
	FString GetDisplayName()
	{
		return DisplayName;
	}
};

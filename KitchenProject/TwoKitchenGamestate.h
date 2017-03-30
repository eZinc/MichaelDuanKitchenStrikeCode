// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "UnrealNetwork.h"
#include "Player/TwoKitchenPlayerState.h"
#include "Kitchen/KitchenManager.h"
#include "TwoKitchenGamestate.generated.h"

/**
 * 
 */
UCLASS()
class KITCHENPROJECT_API ATwoKitchenGamestate : public AGameState
{
	GENERATED_BODY()


	
public:
	ATwoKitchenGamestate();

	UPROPERTY(Replicated)
	TArray<ATwoKitchenPlayerState*> SpectatorArray;

	UPROPERTY(Replicated)
	AKitchenManager* TeamOneKitchen;

	UFUNCTION(BlueprintCallable, Category = Kitchen)
	AKitchenManager* GetTeamOneKitchen()
	{
		return TeamOneKitchen;
	}

	UPROPERTY(Replicated)
	AKitchenManager* TeamTwoKitchen;

	UFUNCTION(BlueprintCallable, Category = Kitchen)
	AKitchenManager* GetTeamTwoKitchen()
	{
		return TeamTwoKitchen;
	}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RespawnTime;

	UPROPERTY(Replicated)
	ATwoKitchenPlayerState* PlayerOnePlayerState;

	UPROPERTY(Replicated)
	ATwoKitchenPlayerState* PlayerTwoPlayerState;

	UPROPERTY(Replicated)
	int32 day = 0;

	UPROPERTY(Replicated)
	int32 minute = 0;

	UPROPERTY(Replicated)
	bool bGameInProgress = false;

	UFUNCTION(BlueprintCallable, Category = GameStats)
	bool GetGameInProgress()
	{
		return bGameInProgress;
	}

	UPROPERTY(Replicated)
	float MinutesPerSecond;

	UFUNCTION(BlueprintCallable, Category = DayNightCycle)
	bool IsNight()
	{
		if (minute >= 1140 || minute < 300)
			return true;
		else
			return false;
	}

	UFUNCTION(BlueprintCallable, Category = DayNightCycle)
	bool IsMorning()
	{
		if (minute >= 300 && minute < 720)
			return true;
		else
			return false;
	}

	UFUNCTION(BlueprintCallable, Category = DayNightCycle)
	bool IsEvening()
	{
		if (minute >= 720 && minute < 1140)
			return true;
		else
			return false;
	}

	UFUNCTION(BlueprintCallable, Category = DayNightCycle)
	int32 GetDay()
	{
		return day;
	}

	UFUNCTION(BlueprintCallable, Category = DayNightCycle)
	int32 GetHour()
	{
		return minute / 60;
	}

	UFUNCTION(BlueprintCallable, Category = DayNightCycle)
	int32 GetMinute()
	{
		return minute;
	}

	UFUNCTION(BlueprintCallable, Category = DayNightCycle)
	int32 GetTimeMinute()
	{
		return (int)minute % 60;
	}

	UFUNCTION(BlueprintCallable, Category = Scoreboard)
	TArray<ATwoKitchenPlayerState*> GetAllSpectators()
	{
		return SpectatorArray;
	}

	void RemoveSpectator(ATwoKitchenPlayerState* p)
	{
		SpectatorArray.Remove(p);
	}

	void AddSpectator(ATwoKitchenPlayerState* p)
	{
		SpectatorArray.Add(p);
	}

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = GameStateData)
	FString GetTeamOneShopName()
	{
		return TeamOneKitchen->GetShopName();
	}

	UFUNCTION(BlueprintCallable, Category = GameStateData)
	FString GetTeamTwoShopName()
	{
		return TeamTwoKitchen->GetShopName();
	}

	UFUNCTION(BlueprintCallable, Category = GameStateData)
	int32 GetTeamOneMoney()
	{
		return TeamOneKitchen->GetMoney();
	}

	UFUNCTION(BlueprintCallable, Category = GameStateData)
	int32 GetTeamTwoMoney()
	{
		return TeamTwoKitchen->GetMoney();
	}

	UFUNCTION(BlueprintCallable, Category = GameStateData)
	int32 GetTeamOneRent()
	{
		return TeamOneKitchen->GetRent();
	}

	UFUNCTION(BlueprintCallable, Category = GameStateData)
	int32 GetTeamTwoRent()
	{
		return TeamTwoKitchen->GetRent();
	}

	UFUNCTION(BlueprintCallable, Category = GameStateData)
	int32 GetTeamOneStrikes()
	{
		return TeamOneKitchen->GetStrikes();
	}

	UFUNCTION(BlueprintCallable, Category = GameStateData)
	int32 GetTeamTwoStrikes()
	{
		return TeamTwoKitchen->GetStrikes();
	}
};

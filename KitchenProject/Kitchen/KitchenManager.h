// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UnrealNetwork.h"
#include "Runtime/Engine/Classes/Engine/TextRenderActor.h"
#include "KitchenManager.generated.h"

UCLASS()
class KITCHENPROJECT_API AKitchenManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AKitchenManager();

	UPROPERTY(EditAnywhere)
	class ADeliveryManager* DeliveryManager;

	// All of the players
	UPROPERTY(Replicated)
	TArray<class ATwoKitchenPlayerController*> PlayerArray;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 Money = 0;

private:

	UPROPERTY(Replicated)
	FString ShopName = "SHOPNAME";

	UPROPERTY(Replicated)
	int32 Strikes = 0;


	UPROPERTY(EditAnywhere, Replicated, Category = Objects)
	TArray<ATextRenderActor*> ShopSigns;

public:
	UPROPERTY(EditDefaultsOnly)
	float CustomerKilledPenalty;

	UPROPERTY(EditDefaultsOnly)
	float CustomerServedBonus;

	UPROPERTY(Replicated)
	int32 Rent = 0;

	// For client/server use
	UPROPERTY(Replicated)
	TArray<class ATwoKitchenPlayerState*> PlayerStateArray;

	TArray<class ATwoKitchenPlayerController*> PlayerControllerArray;

	UFUNCTION(BlueprintCallable, Category = Kitchen)
	TArray<class ATwoKitchenPlayerState*> GetPlayerStateArray()
	{
		return PlayerStateArray;
	}

	void AddRent(int32 in)
	{
		Rent += in;
	}

	void SetRent(int32 in)
	{
		Rent = in;
	}

	UFUNCTION(BlueprintCallable, Category = Kitchen)
	int32 GetRent()
	{
		return Rent;
	}

	void SetStrikes(int32 in)
	{
		Strikes = in;
	}

	void AddStrikes(int32 in)
	{
		Strikes += in;
	}

	UFUNCTION(BlueprintCallable, Category = Kitchen)
	int32 GetStrikes()
	{
		return Strikes;
	}

	UFUNCTION(BlueprintCallable, Category = Kitchen)
	int32 GetMoney()
	{
		return Money;
	}

	void SetMoney(int32 m)
	{
		Money = m;
	}

	void AddMoney(const FString& Reason, int32 m);

	void SetShopName(FString name)
	{
		if (HasAuthority())
		{
			ShopName = name;
			UpdateShopSign(name);
		}
	}

	UFUNCTION(BlueprintCallable, Category = Kitchen)
	FString GetShopName()
	{
		return ShopName;
	}

	UPROPERTY(Replicated, EditAnywhere, Category = Kitchen)
	int32 TeamID;

	UFUNCTION(BlueprintCallable, Category = Kitchen)
	int32 GetTeamID()
	{
		return TeamID;
	}

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	class UCustomerSpawnerComponent* CustomerSpawner;

	UFUNCTION(NetMulticast, Reliable)
	void UpdateShopSign(const FString& ShopNamea);

	// Array of all of the AI Controllers in the kitchen
	TArray<class ATwoKitchenAIController*> CustomerControllers;

	bool AcceptPlayer(class ATwoKitchenPlayerController* contr);
	void RemovePlayer(class ATwoKitchenPlayerController* contr);

	UPROPERTY(EditAnywhere, Category = LineNode)
	class ALineNode* ExitDoor;

	UPROPERTY(EditAnywhere, Category = LineNode)
	TArray<class ALineNode*> StreetLine;

	UPROPERTY(EditAnywhere, Category = LineNode)
	TArray<class ALineNode*> CashierOneLine;

	UPROPERTY(EditAnywhere, Category = LineNode)
	TArray<class ALineNode*> CashierTwoLine;

	UPROPERTY(EditAnywhere, Category = LineNode)
	TArray<class ALineNode*> CashierThreeLine;

	// Take in a customer to the street line
	UFUNCTION(BlueprintCallable, Category = LineNode)
	bool AcceptCustomer(class ATwoKitchenAIController* Customer);

	// Debug Use or actual use idk
	void KickCustomer(class ATwoKitchenAIController* Customer, ALineNode* Node);

	// Returns the best cashier line to go to right now
	void MoveToBestCashierLine(class ATwoKitchenAIController* Customer);

	// Move to the best spot in a given line
	void MoveToBestSpotInLine(class ATwoKitchenAIController* Customer, TArray<class ALineNode*> Line);

	// Tells the entire line to move up
	void MoveLineUp(TArray<class ALineNode*> LineToMove);

	UPROPERTY(Replicated, BlueprintReadOnly)
	float KitchenRating = 0;

	UPROPERTY(EditDefaultsOnly)
	float DefaultKitchenRating;

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastKitchenRatingUpdate(float Change, float Total);

	UFUNCTION(BlueprintImplementableEvent, Category = KitchenRating)
	void KitchenRatingUpdate(float Change, float Total);

	void AddKitchenRating(float in)
	{
		KitchenRating += in;
		if (KitchenRating <= 0)
			KitchenRating = 0.05;

		if (KitchenRating >= 1)
			KitchenRating = 1;

		MulticastKitchenRatingUpdate(in, KitchenRating);
	}

	void CustomerKilled(class ATwoKitchenAICharacter* Victim);

	void MoveLineStartingAt(TArray<class ALineNode*> Line, class ALineNode* Target);

	bool IsKitchenFull();

};

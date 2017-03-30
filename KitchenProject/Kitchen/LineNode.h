// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "../NPC/TwoKitchenAICharacter.h"
#include "../KitchenProjectCharacter.h"
#include "LineNode.generated.h"

UCLASS()
class KITCHENPROJECT_API ALineNode : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALineNode();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	class APawn* Resident;

	bool IsOccupied = false;

	UPROPERTY(EditAnywhere, Category = LineNode)
	bool IsCashierNode;

	UPROPERTY(EditAnywhere, Category = LineNode)
	bool IsDeleteNode;

	UPROPERTY(EditAnywhere, Category = LineNode)
	ALineNode* NextNode;

	bool IsAtCashierNode = false;

	UPROPERTY(EditAnywhere, Category = LineNode)
	class AKitchenManager* KitchenManager;
	
};

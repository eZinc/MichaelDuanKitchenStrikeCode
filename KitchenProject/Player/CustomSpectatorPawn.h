// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SpectatorPawn.h"
#include "CustomSpectatorPawn.generated.h"

/**
 * 
 */
UCLASS()
class KITCHENPROJECT_API ACustomSpectatorPawn : public ASpectatorPawn
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintImplementableEvent, Category = HUD)
	void DrawGameHUD();
	
	
};

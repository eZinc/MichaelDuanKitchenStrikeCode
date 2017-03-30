// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "KitchenItemHealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class KITCHENPROJECT_API UKitchenItemHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category = KitchenItemHealth)
	void InitializeHealthItem(const FString& Name, float MaxHealth, float CurrentHealth, int32 Type);

	UFUNCTION(BlueprintImplementableEvent, Category = KitchenItemHealth)
	void ChangeInHealth(int32 Change, float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = KitchenItemHealth)
	void SetHealthItemVisibility(bool ShouldBeVisible);
	
};

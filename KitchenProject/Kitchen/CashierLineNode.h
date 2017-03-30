// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Kitchen/LineNode.h"
#include "Kitchen/CashRegister.h"
#include "CashierLineNode.generated.h"

/**
 * 
 */
UCLASS()
class KITCHENPROJECT_API ACashierLineNode : public ALineNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = LineNode)
	ACashRegister* CashRegister;

	virtual ACashRegister* GetCashRegister()
	{
		return CashRegister;
	}
	
	
};

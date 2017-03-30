// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UsableInterface.generated.h"

/**
 * 
 */
UINTERFACE()
class UUsableInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class KITCHENPROJECT_API IUsableInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	FString Name = TEXT("NONAME");
	FString Label = TEXT("NOLABEL");
	FString HoldLabel = TEXT("NOLABEL");

	FVector AIUseLocation = FVector(0, 0, 0);
	FRotator AIUseRotation = FRotator(0, 0, 0);
	float AIUseDistance;

public:
	virtual void UseObject(AActor* Instigator);
	virtual void HoldUseObject(AActor* Instigator);

	virtual FString GetHoldLabel()
	{
		return HoldLabel;
	}

	virtual float GetAIUseDistance(AActor* Instigator)
	{
		return AIUseDistance;
	}

	virtual void SetAIUseDistance(float in)
	{
		AIUseDistance = in;
	}

	virtual void SetAIUseLocation( FVector in)
	{
		AIUseLocation = in;
	}

	virtual void SetAIUseRotation(FRotator in)
	{
		AIUseRotation = in;
	}

	virtual FString GetName()
	{
		return Name;
	}

	virtual FString GetLabel()
	{
		return Label;
	}

	virtual void SetName(FString in)
	{
		Name = in;
	}

	virtual void SetLabel(FString in)
	{
		Label = in;
	}

	virtual FVector GetAIUseLocation(AActor* Instigator)
	{
		return AIUseLocation;
	}

	virtual FRotator GetAIUseRotation(AActor* Instigator)
	{
		return AIUseRotation;
	}

	UFUNCTION(BlueprintImplementableEvent, Category = UseInterface)
	void BlueprintHoveredOn();

	UFUNCTION(BlueprintImplementableEvent, Category = UseInterface)
	void BlueprintHoveredOff();

	virtual void HoveredOn()
	{

	}
	virtual void HoveredOff()
	{

	}

};
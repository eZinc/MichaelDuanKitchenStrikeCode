// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "TwoKitchenCharacterMovement.generated.h"

/**
 * 
 */
UCLASS()
class KITCHENPROJECT_API UTwoKitchenCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	void ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration) override;

	
	
};

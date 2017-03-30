// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "DayNightManager.generated.h"

UCLASS()
class KITCHENPROJECT_API ADayNightManager : public AActor
{
	GENERATED_BODY()

protected:

	float MorningSunBrightness;
	float TargetSunBrightness;
	float RequiredCaptureDelta = 0.1f;
	float LastTimeOfDay;
	float TimeSinceLastIncrement;
	float PredictedIncrement;
	float LastCapturedIntensity;

public:	
	// Sets default values for this actor's properties
	ADayNightManager();

	UFUNCTION(BlueprintImplementableEvent, Category = "Lighting")
	void UpdateSkysphere();

	void UpdateSkylight();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, Category = "Lighting")
	ADirectionalLight* DirectionalSunlight;

	UPROPERTY(EditAnywhere, Category = "Lighting")
	ASkyLight* SkyLightActor;
	
};

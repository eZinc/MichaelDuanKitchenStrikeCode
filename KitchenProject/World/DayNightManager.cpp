// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "../TwoKitchenGamestate.h"
#include "DayNightManager.h"


// Sets default values
ADayNightManager::ADayNightManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADayNightManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (DirectionalSunlight)
	{
		MorningSunBrightness = DirectionalSunlight->GetBrightness();
		TargetSunBrightness = MorningSunBrightness;
	}
}

// Called every frame
void ADayNightManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());
	if (GameState)
	{
		if (GameState->bGameInProgress == false)
			return;

		if (DirectionalSunlight)
		{
			if (LastTimeOfDay == GameState->GetMinute())
			{
				TimeSinceLastIncrement += DeltaTime;
			}
			else
			{
				TimeSinceLastIncrement = 0;
			}

			const float PredictedIncrement = GameState->MinutesPerSecond * TimeSinceLastIncrement;
			const float MinutesInDay = 24 * 60;
			const float PitchOffset = 90;
			const float PitchRotation = 360 * ((GameState->GetMinute() + PredictedIncrement) / MinutesInDay);
			
			FRotator NewSunRotation = FRotator(PitchRotation + PitchOffset, 45.0f, 0);
			DirectionalSunlight->SetActorRelativeRotation(NewSunRotation);

			LastTimeOfDay = GameState->GetMinute();
		}

		bool CurrentNightState = GameState->IsNight();
		if (CurrentNightState)
		{
			TargetSunBrightness = 0.01f;
		}
		else
		{
			TargetSunBrightness = MorningSunBrightness;
		}

		const float LerpSpeed = 0.1f * GetWorldSettings()->GetEffectiveTimeDilation();
		float CurrentSunBrightness = DirectionalSunlight->GetBrightness();
		float NewSunBrightness = FMath::Lerp(CurrentSunBrightness, TargetSunBrightness, LerpSpeed);
		DirectionalSunlight->SetBrightness(NewSunBrightness);
	}

	UpdateSkylight();

}

void ADayNightManager::UpdateSkylight()
{
	if (SkyLightActor)
	{
		ATwoKitchenGamestate* GameState = Cast<ATwoKitchenGamestate>(GetWorld()->GetGameState());
		if (GameState)
		{
			const float MinutesInDay = 24 * 60;
			const float Alpha = FMath::Sin(GameState->GetMinute() / MinutesInDay * 3.14);
			const float NewIntensity = FMath::Lerp(0.1, 1.0, Alpha);
			SkyLightActor->GetLightComponent()->SetIntensity(NewIntensity);
			if (RequiredCaptureDelta < FMath::Abs(NewIntensity - LastCapturedIntensity))
			{
				SkyLightActor->GetLightComponent()->RecaptureSky();
				LastCapturedIntensity = NewIntensity;
			}
		}
	}

	UpdateSkysphere();
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "UnrealNetwork.h"
#include "../KitchenProjectCharacter.h"
#include "../Player/TwoKitchenPlayerController.h"
#include "Computer.h"


// Sets default values
AComputer::AComputer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	SetName(TEXT("Computer"));
	SetLabel(TEXT("Use Computer"));
}

// Called when the game starts or when spawned
void AComputer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AComputer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

FString AComputer::GetLabel()
{
	if (bIsInUse)
	{
		return "Computer Being Used!";
	}
	else
	{
		return "Use Computer";
	}
}

void AComputer::UseObject(AActor* Instigator)
{
	if (Instigator->IsA(AKitchenProjectCharacter::StaticClass()))
	{
		AKitchenProjectCharacter* c = Cast<AKitchenProjectCharacter>(Instigator);
		if (c->GetController()->IsA(ATwoKitchenPlayerController::StaticClass()))
		{
			ATwoKitchenPlayerController* cont = Cast<ATwoKitchenPlayerController>(c->GetController());
			RequestUseComputer(cont);
		}
	}
	
}

void AComputer::RequestUseComputer_Implementation(ATwoKitchenPlayerController* cont)
{
	if (HasAuthority())
	{
		if (!bIsInUse)
		{
			cont->ComputerInUse = this;
			bIsInUse = true;
			cont->AllowedUseComputer();
			MulticastShowUsage(true);
		}
	}
}

bool AComputer::RequestUseComputer_Validate(ATwoKitchenPlayerController* cont)
{
	return true;
}

void AComputer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AComputer, bIsInUse);
}

void AComputer::MulticastShowUsage_Implementation(bool ShouldLight)
{
	if (ShouldLight)
	{
		LightUpScreen();
	}
	else
	{
		DarkenScreen();
	}
}

bool AComputer::MulticastShowUsage_Validate(bool ShouldLight)
{
	return true;
}

void AComputer::HoldUseObject(AActor* Instigator)
{

}
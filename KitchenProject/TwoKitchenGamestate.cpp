// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "TwoKitchenGamestate.h"

ATwoKitchenGamestate::ATwoKitchenGamestate()
{
	SpectatorArray.SetNumUninitialized(0);
}

void ATwoKitchenGamestate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATwoKitchenGamestate, PlayerOnePlayerState);
	DOREPLIFETIME(ATwoKitchenGamestate, PlayerTwoPlayerState);
	DOREPLIFETIME(ATwoKitchenGamestate, SpectatorArray);
	DOREPLIFETIME(ATwoKitchenGamestate, minute);
	DOREPLIFETIME(ATwoKitchenGamestate, bGameInProgress);
	DOREPLIFETIME(ATwoKitchenGamestate, MinutesPerSecond);
	DOREPLIFETIME(ATwoKitchenGamestate, day);
	DOREPLIFETIME(ATwoKitchenGamestate, TeamOneKitchen);
	DOREPLIFETIME(ATwoKitchenGamestate, TeamTwoKitchen);

}

// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "Net/UnrealNetwork.h"
#include "TwoKitchenPlayerState.h"




void ATwoKitchenPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATwoKitchenPlayerState, TeamID);
	DOREPLIFETIME(ATwoKitchenPlayerState, DisplayName);
	DOREPLIFETIME(ATwoKitchenPlayerState, KitchenManager);
	DOREPLIFETIME(ATwoKitchenPlayerState, Points);
	DOREPLIFETIME(ATwoKitchenPlayerState, Kills);
	DOREPLIFETIME(ATwoKitchenPlayerState, Deaths);
	DOREPLIFETIME(ATwoKitchenPlayerState, Assists);
	DOREPLIFETIME(ATwoKitchenPlayerState, Mobscore);
	DOREPLIFETIME(ATwoKitchenPlayerState, IsReady);
}

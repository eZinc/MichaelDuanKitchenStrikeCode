// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "DeliveryZone.h"


// Sets default values
ADeliveryZone::ADeliveryZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADeliveryZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeliveryZone::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "LineManager.h"


// Sets default values
ALineManager::ALineManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALineManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALineManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}


// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "../NPC/TwoKitchenAIController.h"
#include "LineNode.h"


// Sets default values
ALineNode::ALineNode()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ALineNode::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALineNode::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}
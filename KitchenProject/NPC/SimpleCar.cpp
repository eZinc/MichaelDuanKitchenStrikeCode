// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "../KitchenProjectCharacter.h"
#include "UnrealNetwork.h"
#include "SimpleCar.h"


// Sets default values
ASimpleCar::ASimpleCar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASimpleCar::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &ASimpleCar::Despawn, Lifetime, false);

}

void ASimpleCar::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	UGameplayStatics::SpawnSoundAttached(HitSound, GetRootComponent());

	if (HasAuthority())
	{
		FVector ImpulseDirection = Other->GetActorLocation() - GetActorLocation();
		ImpulseDirection.Normalize();

		if (Other->IsA(AKitchenProjectCharacter::StaticClass()))
		{
			AKitchenProjectCharacter* c = Cast <AKitchenProjectCharacter>(Other);
			c->GetCharacterMovement()->Launch(ImpulseDirection * ImpulseStrength);
			c->CustomTakeDamage(false, ImpulseDirection * ImpulseStrength, Other->GetActorLocation(), HitDamage, FDamageEvent(), NULL, this);
		}
		else
		{
			MyComp->AddImpulse(ImpulseDirection * ImpulseStrength);
		}
	}
}

void ASimpleCar::Despawn()
{
	Destroy();
}

// Called every frame
void ASimpleCar::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	FVector forward = GetActorForwardVector();
	AddActorWorldOffset(forward * Speed * DeltaTime, true);
}


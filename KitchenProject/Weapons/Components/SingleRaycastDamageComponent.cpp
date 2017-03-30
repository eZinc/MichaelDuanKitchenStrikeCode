// Fill out your copyright notice in the Description page of Project Settings.

#include "KitchenProject.h"
#include "../../KitchenProjectCharacter.h"
#include "UnrealNetwork.h"
#include "../SimpleRaycastWeapon.h"
#include "WeaponDataComponent.h"
#include "../../DamageableInterface.h"
#include "SingleRaycastDamageComponent.h"


// Sets default values for this component's properties
USingleRaycastDamageComponent::USingleRaycastDamageComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USingleRaycastDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USingleRaycastDamageComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}

bool USingleRaycastDamageComponent::ShootRaycast(UWeaponDataComponent* data, FVector MuzzleLocation, FVector ForwardVector, FVector StartingPosition, AKitchenProjectCharacter* c)
{
	// If there's no ammo
	if (data->CurrentMag <= 0)
	{
		// Perhaps play a sound
		return false;
	}

	static FName WeaponFireTag = FName(TEXT("Weapon"));
	// Start from the muzzle's position
	FVector StartPos = StartingPosition;
	// Get forward vector of MyPawn
	FVector Forward = ForwardVector;
	// Calculate end position
	FVector EndPos = StartPos + Forward * Range;
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(WeaponFireTag, false, GetOwner());
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.AddIgnoredActor(c);

	// This fires the ray and checks against all objects w/ collision
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartPos, EndPos,
		ECC_Visibility, TraceParams);

	// Immediately have a tracer because it feels good clientside
	RaycastEffect(MuzzleLocation, Hit);

	// Tell the server to calculate raycast and present to everyone else
	ServerRequestRaycast(data, MuzzleLocation, Hit, ForwardVector);

	// Clientside reduce mag for clientside UI purposes
	if(!GetOwner()->HasAuthority())
		data->CurrentMag = data->CurrentMag - 1;

	return true;
}

void USingleRaycastDamageComponent::RaycastEffect(FVector MuzzleLocation, FHitResult Hit)
{
	UGameplayStatics::SpawnSoundAtLocation(GetWorld(), FireSound, MuzzleLocation);
	
	FVector RealMuzzleLocation = MuzzleLocation;
	AKitchenProjectCharacter* c = Cast<AKitchenProjectCharacter>(GetOwner()->GetOwner());
	if (c)
	{
		// If not owning player then change to third person meshs' muzzle
		if (c->GetController() != GetWorld()->GetFirstPlayerController())
		{
			ASimpleRaycastWeapon* w = Cast<ASimpleRaycastWeapon>(GetOwner());
			RealMuzzleLocation = w->ThirdPersonWeapon->ThirdPersonWeaponMesh->GetSocketLocation(w->SocketName);
		}
	}


	TrailComponent = UGameplayStatics::SpawnEmitterAtLocation(this, TracerParticle, RealMuzzleLocation);
	TrailComponent->SetBeamTargetPoint(0, Hit.TraceEnd, 0);
	TrailComponent->SetBeamSourcePoint(0, RealMuzzleLocation, 0);

	if (Hit.bBlockingHit)
	{
		TrailComponent->SetBeamTargetPoint(0, Hit.Location, 0);

		// TODO : Different impact effects for hitting different actors
		FVector Normal = Hit.ImpactNormal;
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, Hit.ImpactPoint, Normal.Rotation());
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), BrickImpactSound, Hit.ImpactPoint);
	}
}

void USingleRaycastDamageComponent::ServerRequestRaycast_Implementation(UWeaponDataComponent* data, FVector MuzzleLocation, FHitResult Hit, FVector ForwardVector)
{
	// If no ammo definitely don't shoot
	if (data->CurrentMag <= 0)
		return;

	// Deplete one ammo
	data->CurrentMag = data->CurrentMag - 1;

	if (Hit.bBlockingHit)
	{
		float TempImpulseAmount = ImpulseAmount;
		if (Hit.GetActor())
		{
			IDamageableInterface* u = Cast<IDamageableInterface>(Hit.GetActor());

			if (u)
			{
				AKitchenProjectCharacter* d = Cast<AKitchenProjectCharacter>(GetOwner()->GetOwner());
				
				float FinalDamage = Damage;

				FVector TraceLine = Hit.Location - Hit.TraceStart;
				if (TraceLine.Size() > FalloffDistance)
				{
					float extra = TraceLine.Size() - FalloffDistance;
					FinalDamage -= extra * DamageLostPerUnit;
					if (FinalDamage <= 0)
						FinalDamage = 1;
				}

				bool bDidCrit = false;
				if (Hit.BoneName == FName("head"))
				{
					FinalDamage *= 2;
					bDidCrit = true;
					TempImpulseAmount *= 2;
				}

				float returned = u->CustomTakeDamage(bDidCrit, ForwardVector * TempImpulseAmount, Hit.ImpactPoint, FinalDamage, FDamageEvent(), d->GetController(), d);
			}
			else
			{
				// Shoots props around
				if (Hit.GetComponent()->IsSimulatingPhysics())
				{
					Hit.GetComponent()->AddImpulseAtLocation(ForwardVector * ImpulseAmount, Hit.ImpactPoint, NAME_None);
				}
			}
		}

	}

	MulticastRaycastEffect(MuzzleLocation, Hit);
}

bool USingleRaycastDamageComponent::ServerRequestRaycast_Validate(UWeaponDataComponent* data, FVector MuzzleLocation, FHitResult Hit, FVector ForwardVector)
{
	return true;
}

void USingleRaycastDamageComponent::MulticastRaycastEffect_Implementation(FVector MuzzleLocation, FHitResult Hit)
{
	AKitchenProjectCharacter* c = Cast<AKitchenProjectCharacter>(GetOwner()->GetOwner());
	if (c)
	{
		// return if owning player
		if (c->GetController() == GetWorld()->GetFirstPlayerController())
			return;
	}

	RaycastEffect(MuzzleLocation, Hit);
}
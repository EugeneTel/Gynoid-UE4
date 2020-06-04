// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initializing components
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::Fire()
{
	if (!CanShoot())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Shoot. Weapon state: %s"), State);
		return;
	}

	
}

//----------------------------------------------------------------------------------------------------------------------
// Checking does the Weapon can shoot?
//----------------------------------------------------------------------------------------------------------------------
bool AWeapon::CanShoot() const
{
	return State == EWeaponState::EWS_Ready;
}

//----------------------------------------------------------------------------------------------------------------------
// Checking does the Weapon has Ammo in a Mag?
//----------------------------------------------------------------------------------------------------------------------
bool AWeapon::HasAmmo() const
{
	return CurrentAmmo > 0;
}

//----------------------------------------------------------------------------------------------------------------------
// The Weapon makes one Shot
//----------------------------------------------------------------------------------------------------------------------
void AWeapon::OneShot()
{
	if (HasAmmo())
	{
		
	}
}

void AWeapon::SpawnProjectile()
{
}

FTransform AWeapon::CalcProjectileDirection()
{
	return FTransform(FVector(0.f));
}

//----------------------------------------------------------------------------------------------------------------------
// Increase the current magazines of the weapon
//----------------------------------------------------------------------------------------------------------------------
void AWeapon::IncreaseCurrentMags(const int32 MagsToAdd)
{
	(CurrentMags + MagsToAdd < MaxMags) ? CurrentMags += MagsToAdd : CurrentMags = MaxMags;
}


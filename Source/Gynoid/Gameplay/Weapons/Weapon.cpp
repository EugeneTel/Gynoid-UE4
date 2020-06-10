// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/AudioComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gynoid/Gameplay/Interfaces/WeaponableInterface.h"
#include "GameFramework/Character.h"
#include "Gynoid/Gynoid.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectiles/Projectile.h"
#include "Sound/SoundCue.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Initializing components
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComp);

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComp->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
}

UAudioComponent* AWeapon::PlayWeaponSound(USoundCue* Sound) const
{
	UAudioComponent* AC = nullptr;
	if (Sound && WeaponOwner)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, WeaponOwner->GetRootComponent());
	}

	return AC;
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeapon::SetOwningPawn(ACharacter* NewPawn)
{
	WeaponOwner = NewPawn;
	SetInstigator(NewPawn);

	if (NewPawn->GetClass()->ImplementsInterface(UWeaponableInterface::StaticClass()))
	{
		OwnerCamera = IWeaponableInterface::Execute_GetCamera(WeaponOwner);
		if (!OwnerCamera)
		{
			UE_LOG(LogTemp, Error, TEXT("The GetCamera method is not implemented or returns empty result for WeaponableInteface!"));
		}
	} else
	{
		UE_LOG(LogTemp, Error, TEXT("The Character Must implement WeaponableInterface interface for the Weapon System."));
	}
}

void AWeapon::StartFire()
{
	if (!CanShoot())
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't Shoot. Weapon state: %s"), State);
		return;
	}

	OneShot();
}

void AWeapon::StopFire()
{
}

//----------------------------------------------------------------------------------------------------------------------
// Weapon Trace try to find a hit
//----------------------------------------------------------------------------------------------------------------------
FHitResult AWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo, bool bDebug) const
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	if (bDebug)
	{
		DrawDebugLine(GetWorld(), TraceFrom, TraceTo, FColor::Green, false, 5, 0, 1);	
	}

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, COLLISION_WEAPON, TraceParams);
	
	return Hit;
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
	
	const FVector StartTrace = GetProjectileStartPoint();
	const FVector EndTrace = GetProjectileEndPoint();

	FVector Origin = GetMuzzleLocation();
	FVector ShootDir = EndTrace; 

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace, bDebugMode);

	if (Impact.bBlockingHit)
	{
		ShootDir = Impact.ImpactPoint;
	}

	FRotator ShootRotation = UKismetMathLibrary::FindLookAtRotation(Origin, ShootDir);

	FireProjectile(Origin, ShootRotation);

	PlayWeaponSound(FireSound);
	/*
	if (HasAmmo())
	{
		
	}*/
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

FVector AWeapon::GetProjectileStartPoint() const
{
	return OwnerCamera->GetComponentLocation();
}

FVector AWeapon::GetProjectileEndPoint()
{
	UCameraComponent* CameraComp = IWeaponableInterface::Execute_GetCamera(WeaponOwner);
	if (!CameraComp)
	{
		UE_LOG(LogTemp, Error, TEXT("The GetCamera method is not implemented or returns empty result for WeaponableInteface!"));
		return FVector::ZeroVector;
	}

	return CameraComp->GetComponentLocation() + CameraComp->GetForwardVector() * FVector(10000.f);
}

FVector AWeapon::GetMuzzleLocation() const
{
	return MeshComp->GetSocketLocation(MuzzleSocketName);
}

void AWeapon::FireProjectile(FVector Origin, FRotator ShootRotation)
{
	const FVector Offset = OwnerCamera->GetForwardVector() * ProjectileSpawnOffset;
	const FTransform SpawnTransform(ShootRotation, Origin);
	AProjectile* NewProjectile = Cast<AProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTransform));
	if (NewProjectile)
	{
		NewProjectile->SetInstigator(GetInstigator());
		NewProjectile->SetOwner(this);
		//NewProjectile->InitVelocity(ShootDir);
		NewProjectile->SetProjectileData(ProjectileConfig);

		UGameplayStatics::FinishSpawningActor(NewProjectile, SpawnTransform);
	}
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"
#include "Gynoid/Gameplay/Interfaces/WeaponableInterface.h"
#include "Gynoid/Gameplay/Weapons/WeaponComponent.h"

// Sets default values
AAmmoPickup::AAmmoPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComp);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetupAttachment(CollisionComp);

}

// Called when the game starts or when spawned
void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AAmmoPickup::OnOverlap);
}

// Called every frame
void AAmmoPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

EWeaponType AAmmoPickup::GetWeaponType() const
{
	return WeaponType;
}

// Called when the box component overlapped
void AAmmoPickup::OnOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{

	// Try to pickup the ammo
	if (OtherActor->GetClass()->ImplementsInterface(UWeaponableInterface::StaticClass()))
	{
		UWeaponComponent* WeaponComp = IWeaponableInterface::Execute_GetWeaponComponent(OtherActor);

		if (WeaponComp)
		{
			WeaponComp->PickupAmmo(this);
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "Gynoid/Gameplay/Interfaces/WeaponableInterface.h"
#include "Weapon.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	WeaponOwner = GetOwner();

	SpawnStartWeapons();
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//----------------------------------------------------------------------------------------------------------------------
// Returns a reference of the desired weapon
//----------------------------------------------------------------------------------------------------------------------
AWeapon* UWeaponComponent::GetWeaponByType(EWeaponType WeaponType) const
{
	for (auto It = WeaponList.CreateConstIterator(); It; ++It)
	{
		if ((*It) && (*It)->GetType() == WeaponType)
		{
			return *It;
		}
	}

	// didn't find a valid reference
	return nullptr;
}

//----------------------------------------------------------------------------------------------------------------------
// Spawn a desired Weapon
//----------------------------------------------------------------------------------------------------------------------
void UWeaponComponent::SpawnWeapon(TSubclassOf<AWeapon> WeaponClass)
{
	FActorSpawnParameters ActorSpawnParams;
	ActorSpawnParams.Owner = WeaponOwner;

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("The World is not avalable!"));
		return;
	}

	if (!WeaponOwner->GetClass()->ImplementsInterface(UWeaponableInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Error, TEXT("The Character Must implement IWeaponable interface for the Weapon System."));
		return;
	}

	// spawn weapon
	AWeapon* WeaponRef = World->SpawnActor<AWeapon>(WeaponClass->GetDefaultObject()->GetClass(), GetOwner()->GetTransform(), ActorSpawnParams);
	//WeaponRef->SetActorHiddenInGame(true);

	const FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);

	USkeletalMeshComponent* OwnerMesh = IWeaponableInterface::Execute_GetMesh(WeaponOwner);
	if (!OwnerMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("The GetMesh method is not implemented or returns empty result for WeaponableInteface!"));
		return;
	}
	
	WeaponRef->AttachToComponent(OwnerMesh, AttachmentTransformRules, WeaponRef->GetCharacterSocket());
	WeaponRef->SetActorRelativeTransform(WeaponRef->GetAttachRelativeTransform());

	WeaponList.Add(WeaponRef);

	UE_LOG(LogTemp, Warning, TEXT("The Weapon Successfully Spawned"));
}

//----------------------------------------------------------------------------------------------------------------------
// Spawn all weapons from StartWeaponClasses
//----------------------------------------------------------------------------------------------------------------------
void UWeaponComponent::SpawnStartWeapons()
{
	for (auto It = StartWeaponClasses.CreateConstIterator(); It; ++It)
	{
		SpawnWeapon(*It);
	}
}

//----------------------------------------------------------------------------------------------------------------------
// Equip weapon for the character
//----------------------------------------------------------------------------------------------------------------------
void UWeaponComponent::EquipWeapon(EWeaponType WeaponType)
{
	
}


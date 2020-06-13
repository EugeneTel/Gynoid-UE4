// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "Gynoid/Gameplay/Interfaces/WeaponableInterface.h"
#include "Weapon.h"
#include "GameFramework/Character.h"

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

	WeaponOwner = Cast<ACharacter>(GetOwner());
	check(WeaponOwner)

	// SpawnDefaultInventory at the next tick
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UWeaponComponent::SpawnDefaultInventory);
}

// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//----------------------------------------------------------------------------------------------------------------------
// Weapon usage
//----------------------------------------------------------------------------------------------------------------------
#pragma region Weapon

void UWeaponComponent::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}

void UWeaponComponent::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

#pragma endregion Weapon

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
#pragma region Inventory

void UWeaponComponent::AddWeapon(AWeapon* Weapon)
{
	Weapon->OnEnterInventory(WeaponOwner);
	Inventory.AddUnique(Weapon);
}

void UWeaponComponent::RemoveWeapon(AWeapon* Weapon)
{
	Weapon->OnLeaveInventory();
	Inventory.RemoveSingle(Weapon);
}

AWeapon* UWeaponComponent::GetWeapon() const
{
	return CurrentWeapon;
}

AWeapon* UWeaponComponent::FindWeapon(const TSubclassOf<AWeapon> WeaponClass)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] && Inventory[i]->IsA(WeaponClass))
		{
			return Inventory[i];
		}
	}

	return nullptr;
}

void UWeaponComponent::EquipWeapon(AWeapon* Weapon)
{
	if (Weapon)
	{
		SetCurrentWeapon(Weapon, CurrentWeapon);
	}
}

void UWeaponComponent::SpawnDefaultInventory()
{
	const int32 NumWeaponClasses = DefaultInventoryClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AWeapon* NewWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultInventoryClasses[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

void UWeaponComponent::SetCurrentWeapon(AWeapon* NewWeapon, AWeapon* LastWeapon)
{
	AWeapon* LocalLastWeapon = nullptr;

	if (LastWeapon != nullptr)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		NewWeapon->SetWeaponOwner(WeaponOwner);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!

		NewWeapon->OnEquip(LastWeapon);
	}
}

void UWeaponComponent::DestroyInventory()
{
	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		AWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}

FName UWeaponComponent::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}



// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponComponent.generated.h"

enum class EWeaponType : uint8;
class AWeapon;
class AAmmoPickup;

/**
 * Weapon Component
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GYNOID_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	// Sets default values for this component's properties
	UWeaponComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
//----------------------------------------------------------------------------------------------------------------------
// General
//----------------------------------------------------------------------------------------------------------------------
protected:
	
	/** The Character Ref must implement WeaponableInterface */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(MustImplement="WeaponableInterface"), Category=Info)
    ACharacter* WeaponOwner;

//----------------------------------------------------------------------------------------------------------------------
// Weapon Usage
//----------------------------------------------------------------------------------------------------------------------
#pragma region Weapon
protected:

	/** current firing state */
	bool bWantsToFire;
	
public:

	/** starts weapon fire */
	UFUNCTION(BlueprintCallable)
    void StartWeaponFire();

	/** stops weapon fire */
	UFUNCTION(BlueprintCallable)
    void StopWeaponFire();
	
#pragma endregion Weapon
	
//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
#pragma region Inventory
protected:

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	FName WeaponAttachPoint;
	
	/** Weapons array. It's used to hold a reference to all available weapons */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category=Inventory)
	TArray<AWeapon*> Inventory;

	/** The Character will start play with the list of weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Inventory)
    TArray<TSubclassOf<AWeapon>> DefaultInventoryClasses;

	/** Currently equipped weapon */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=Inventory)
    AWeapon* CurrentWeapon;

public:

	/** add weapon to inventory */
	UFUNCTION(BlueprintCallable)
	void AddWeapon(AWeapon* Weapon);

	/** remove weapon from inventory */
	UFUNCTION(BlueprintCallable)
	void RemoveWeapon(AWeapon* Weapon);

	/** reload the current weapon */
	UFUNCTION(BlueprintCallable)
	void ReloadWeapon() const;

	/** get currently equipped weapon */
	UFUNCTION(BlueprintCallable)
	AWeapon* GetWeapon() const;

	/** Find in inventory by weapon class */
	AWeapon* FindWeapon(TSubclassOf<AWeapon> WeaponClass);

	/** Find in inventory by weapon type*/
	UFUNCTION(BlueprintCallable)
    AWeapon* FindWeapon(EWeaponType WeaponType);
	
	/** equips weapon from inventory */
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(AWeapon* Weapon);

	/** equips the next weapon from inventory */
	UFUNCTION(BlueprintCallable)
	void NextWeapon();

	/** equips the previous weapon from inventory */
	UFUNCTION(BlueprintCallable)
	void PrevWeapon();

	/** spawns default inventory */
	void SpawnDefaultInventory();

	/** updates current weapon */
	void SetCurrentWeapon(AWeapon* NewWeapon, class AWeapon* LastWeapon = nullptr);

	/** remove all weapons from inventory and destroy them */
	void DestroyInventory();

	/** get weapon attach point */
	FName GetWeaponAttachPoint() const;

	/** pickup the ammo */
	UFUNCTION(BlueprintCallable)
    void PickupAmmo(AAmmoPickup* AmmoPickup);
	
#pragma endregion Inventory
	
};

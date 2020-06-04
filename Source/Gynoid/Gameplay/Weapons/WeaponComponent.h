// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapon.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GYNOID_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	/** The Character Ref must implement WeaponableInterface */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(MustImplement="WeaponableInterface"), Category = "Info")
    AActor* WeaponOwner;

	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info")
	TScriptInterface<IWeaponableInterface> WeaponableOwner;*/
	
	/** Weapons array. It's used to hold a reference to all available weapons */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Info")
    TArray<AWeapon*> WeaponList;

	/** The Character will start play with the list of weapons */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
    TArray<TSubclassOf<AWeapon>> StartWeaponClasses;

	/**
	* Returns a reference of the desired weapon 
	*/
	UFUNCTION(BlueprintCallable)
    AWeapon* GetWeaponByType(EWeaponType WeaponType) const;

	/**
	* Spawn a desired weapon
	*/
	UFUNCTION(BlueprintCallable)
    void SpawnWeapon(TSubclassOf<AWeapon> WeaponClass);

	/**
	 * Spawn all weapons from StartWeaponClasses
	 */
	UFUNCTION(BlueprintCallable)
	void SpawnStartWeapons();
	
public:
	// Sets default values for this component's properties
	UWeaponComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Makes the character to equip the desired weapon if possible */
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(EWeaponType WeaponType);
};

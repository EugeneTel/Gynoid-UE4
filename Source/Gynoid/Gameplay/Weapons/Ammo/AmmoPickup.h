// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Gynoid/Gameplay/Weapons/Weapon.h"

#include "AmmoPickup.generated.h"

UCLASS()
class GYNOID_API AAmmoPickup : public AActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UBoxComponent* CollisionComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComp;

	/** Ammo fits only for the selected Weapon Type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType WeaponType;

	/** How many ammo will be added */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Amount;
	
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

	/**
	* Called when the box component overlapped
	*/
	UFUNCTION()
    void OnOverlap(
        UPrimitiveComponent* OverlappedComp, 
        AActor* OtherActor, 
        UPrimitiveComponent* OtherComp, 
        int32 OtherBodyIndex, 
        bool bFromSweep, 
        const FHitResult& SweepResult
    );
	
public:	
	// Sets default values for this actor's properties
	AAmmoPickup();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Get ammo weapon type */
	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeaponType() const;

	/** Get Ammo Amount */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetAmount() const { return Amount; }

};

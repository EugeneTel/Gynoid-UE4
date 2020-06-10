// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gynoid/Gameplay/Weapons/Weapon.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

/**
* Base Projectile Class. Must be implemented by other projectiles
*/
UCLASS(Abstract)
class GYNOID_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** initial setup */
	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** The damage amount that this projectile applies to hit enemies */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	float Damage;

	/** This component is just a  wrapper for collision generation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Config)
    USphereComponent* CollisionComp;

	/** The component responsible for the movement of the projectile */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProjectileMovementComponent* MovementComp;

	/** The particle system for the current projectile */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Config)
    UParticleSystemComponent* ParticleComp;

	/** Projectile Data */
	FProjectileWeaponData ProjectileData;

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

	/**
	 * Called when the projectile stops
	 */
	UFUNCTION()
    void OnImpact(const FHitResult& ImpactResult);

public:	

	/** setup velocity */
	void InitVelocity(FVector& ShootDirection) const;

	void SetProjectileData(FProjectileWeaponData& InProjectileData);

};

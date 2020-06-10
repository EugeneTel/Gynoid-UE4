// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.generated.h"

class AProjectile;
class UAudioComponent;
class USoundCue;
class UCameraComponent;

/**
 * Weapon Types
 */
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Pistol UMETA(DisplayName = "Pistol"),
	EWT_Rifle UMETA(DisplayName = "Rifle"),

	EWT_Max UMETA(DisplayName = "DefaultMax")
};

/**
* Weapon State
*/
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Ready UMETA(DisplayName = "Ready"),
    EWS_Reloading UMETA(DisplayName = "Reloading"),
	EWS_NoAmmo UMETA(DisplayName = "NoAmmo"),
	EWS_FireRateDelay UMETA(DisplayName = "FireRateDelay"),

    EWS_Max UMETA(DisplayName = "DefaultMax")
};

/**
* Weapon Fire Mode
*/
UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	EWFM_Single UMETA(DisplayName = "Single Shot"),
    EWFM_Auto UMETA(DisplayName = "Auto Shooting"),

    EWFM_Max UMETA(DisplayName = "DefaultMax")
};

USTRUCT(BlueprintType)
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

	/** projectile class */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<AProjectile> ProjectileClass;

	/** life time */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float ProjectileLife;

	/** initial speed */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float InitialSpeed;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<UDamageType> DamageType;

	/** defaults */
	FProjectileWeaponData()
	{
		ProjectileClass = nullptr;
		ProjectileLife = 10.f;
		InitialSpeed = 10000.f;
		DamageType = UDamageType::StaticClass();
	}
};

/**
 * Base Weapon Class. Must be implemented by other weapons
 */
UCLASS(Abstract)
class GYNOID_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Skeletal mesh of the weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Config)
    USkeletalMeshComponent* MeshComp;

	/** The weapon type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Config)
	EWeaponType Type;

	/** The weapon fire mode */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Config)
	EWeaponFireMode FireMode;

	/** Enable Debug Mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Config)
	bool bDebugMode;

	/** Current State of the Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Info)
    EWeaponState State;
	
	/** The socket's name of the character's skeletal mesh that we weapon is going to get attached to */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Config)
    FName SocketToAttach;

	/** The weapon relative position and relative rotation for attachment */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Config)
    FTransform AttachRelativeTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Config)
    FName MuzzleSocketName;
	
	/**
	* Get the muzzle location of the weapon
	*/
	FVector GetMuzzleLocation() const;

//----------------------------------------------------------------------------------------------------------------------
// Weapon Owner
//----------------------------------------------------------------------------------------------------------------------
	UPROPERTY()
	ACharacter* WeaponOwner;

	UPROPERTY()
	UCameraComponent* OwnerCamera;

//----------------------------------------------------------------------------------------------------------------------
// Audio
//----------------------------------------------------------------------------------------------------------------------

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Config)
	UAudioComponent* AudioComp;

	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
    USoundCue* FireSound;

	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound) const;

//----------------------------------------------------------------------------------------------------------------------
// Ammo
//----------------------------------------------------------------------------------------------------------------------

	/** Maximum Ammo */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 MaxAmmo;

	/** Current Ammo. Set up as initial Ammo too */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
    int32 CurrentAmmo;

	/** Maximum Magazines */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
	int32 MaxMags;

	/** Current Magazines */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
    int32 CurrentMags;

	/** The weapon fire rate in seconds, ie 1 means that the weapon can fire every second. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Stats)
    float FireRate;

	/**
	* Checking does the Weapon has Ammo in a Mag?
	*/
	UFUNCTION(BlueprintCallable)
    bool HasAmmo() const;

	/**
	* Increases the current magazines
	*/
	UFUNCTION(BlueprintCallable)
    void IncreaseCurrentMags(int32 MagsToAdd);

//----------------------------------------------------------------------------------------------------------------------
// Projectile
//----------------------------------------------------------------------------------------------------------------------
	
	/** The projectile that will be spawned at the weapon's muzzle */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
    TSubclassOf<AProjectile> Projectile;
	
	/** Weapon config */
	UPROPERTY(EditDefaultsOnly, Category=Config)
    FProjectileWeaponData ProjectileConfig;

	/** The max distance that the Projectile of the Weapon will travel */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Projectile)
    float ProjectileMaxDistance;
	
	/** Projectile's speed multiplier */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
    float ProjectileSpeedMultiplier;

	/** Projectile's damage multiplier */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
    float ProjectileDamageMultiplier;
	
	/** The projectiles should get spawned a bit infront of the muzzle so they won't get stuck */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
    float ProjectileSpawnOffset;
	
	/**
	* Spawn the Projectile
	*/
	void SpawnProjectile();
	
	/**
	* Calculate Projectile Start Point
	*/
	FVector GetProjectileStartPoint() const;

	/**
	* Calculate projectile End Point
	*/
	FVector GetProjectileEndPoint();

	/**
	*	Calculate the main vector for the Projectile
	*/
	FTransform CalcProjectileDirection();
	
	/**
	* Fire the projectile 
	*/
	void FireProjectile(FVector Origin, FRotator ShootRotation);

//----------------------------------------------------------------------------------------------------------------------
// Shooting
//----------------------------------------------------------------------------------------------------------------------
	/**
	 * Checks does the Weapon can shoot?
	 */
	UFUNCTION(BlueprintCallable)
	bool CanShoot() const;

	/**
	 * The Weapon makes one Shot
	 */
	void OneShot();

	/** Find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo, bool bDebug = false) const;

public:
	/**
	* Fires the weapon	
	*/
	UFUNCTION(BlueprintCallable)
    void StartFire();

	UFUNCTION(BlueprintCallable)
    void StopFire();

	



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns the current ammo of the weapon */
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }

	/** Returns the current ammo of the weapon */
	FORCEINLINE int32 GetCurrentMags() const { return CurrentMags; }

	/** Returns the weapon type */
	FORCEINLINE EWeaponType GetType() const { return Type; }

	/** Returns the attached socket name */
	FORCEINLINE FName GetCharacterSocket() const { return SocketToAttach; }

	/** Returns the skeletal mesh of the weapon */
	FORCEINLINE USkeletalMeshComponent* GetMeshComponent() const { return MeshComp; }

	/** Returns the weapon muzzle socket name */
	FORCEINLINE FName GetMuzzleSocketName() const { return MuzzleSocketName; }

	/** Returns Transform for Attachment */
	FORCEINLINE FTransform GetAttachRelativeTransform() const { return AttachRelativeTransform; }

	/** Returns the weapon state */
	FORCEINLINE EWeaponState GetState() const { return State; }

	UFUNCTION(BlueprintCallable)
	void SetOwningPawn(ACharacter* NewPawn);

};

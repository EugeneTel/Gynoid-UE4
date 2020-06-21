// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.generated.h"

class AProjectile;
class UAudioComponent;
class USoundCue;
class UCameraComponent;
class UCameraShake;
class UDamageType;
class UAnimMontage;
class UWeaponComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWeaponUpdateAmmo, int32&, int32&);

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
	EWS_Idle UMETA(DisplayName = "Idle"),
    EWS_Firing UMETA(DisplayName = "Firing"),
	EWS_Reloading UMETA(DisplayName = "Reloading"),
	EWS_Equipping UMETA(DisplayName = "Equipping"),

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

/**
 * Projectile Weapon Data
 */
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
 *	Weapon Data
 */
USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

	/** max ammo */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
    int32 MaxAmmo;

	/** clip size */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
    int32 AmmoPerClip;

	/** initial clips */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
    int32 InitialClips;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
    float TimeBetweenShots;

	/** failsafe reload duration if weapon doesn't have any animation for it */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
    float NoAnimReloadDuration;

	/** attachment offset for correct position */
	UPROPERTY(EditDefaultsOnly, Category=Config)
	FTransform AttachOffset;

	/** animation played on pawn */
	UPROPERTY(EditDefaultsOnly, Category=Animation)
    UAnimMontage* AnimMontage;

	/** defaults */
	FWeaponData()
	{
		MaxAmmo = 100;
		AmmoPerClip = 20;
		InitialClips = 4;
		TimeBetweenShots = 0.2f;
		NoAnimReloadDuration = 1.0f;
		AnimMontage = nullptr;
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

	/** perform initial setup */
	virtual void PostInitializeComponents() override;
	
	/** Global notification when a character updates ammo. Needed for HUD */
	static FOnWeaponUpdateAmmo NotifyUpdateAmmo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	

//----------------------------------------------------------------------------------------------------------------------
// General
//----------------------------------------------------------------------------------------------------------------------
#pragma region General
protected:

	/** Skeletal mesh of the weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=General)
	USkeletalMeshComponent* MeshComp;

	/** The weapon type */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=General)
    EWeaponType Type;
	
	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category=General)
	FWeaponData WeaponConfig;

	/** projectile data */
	UPROPERTY(EditDefaultsOnly, Category=General)
	FProjectileWeaponData ProjectileConfig;

    /** The weapon owner character */
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=General)
    ACharacter* WeaponOwner;

	/** The owner weapon component */ 
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category=General)
	UWeaponComponent* OwnerComp;

	/** current weapon state */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category=General)
	EWeaponState CurrentState;

public:
    /** set the weapon's owning pawn */
    void SetWeaponOwner(ACharacter* NewOwner);

	/** Get current weapon's state */
	UFUNCTION(BlueprintCallable)
	EWeaponState GetCurrentState() const;

	/** Get current weapon's type */
	UFUNCTION(BlueprintCallable)
	EWeaponType GetType() const;

#pragma endregion General

//----------------------------------------------------------------------------------------------------------------------
// Firing
//----------------------------------------------------------------------------------------------------------------------
#pragma region Firing
protected:
	/** firing audio (bLoopedFireSound set) */
	UPROPERTY(Transient)
	UAudioComponent* FireAudioComp;

	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
    USoundCue* FireSound;

	/** looped fire sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
    USoundCue* FireLoopSound;

	/** finished burst sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
    USoundCue* FireFinishSound;

	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
    FName MuzzleSocketName;

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
    UParticleSystem* MuzzleFX;

	/** Scale for MuzzleFX */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
	FVector MuzzleFXScale;

	/** spawned component for muzzle FX */
	UPROPERTY(Transient)
    UParticleSystemComponent* MuzzleParticleComp;

	/** camera shake on firing */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
    TSubclassOf<class UCameraShake> FireCameraShake;

	/** is fire animation looped? */
	UPROPERTY(EditDefaultsOnly, Category=Animation)
    bool bLoopedFireAnim;
	
	/** fire animations */
	UPROPERTY(EditDefaultsOnly, Category=Animation)
    UAnimMontage* FireAnim;

	/** is muzzle FX looped? */
	UPROPERTY(EditDefaultsOnly, Category=Effects)
    bool bLoopedMuzzleFX;

	/** is fire sound looped? */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
    bool bLoopedFireSound;

	/** is fire animation playing? */
	bool bPlayingFireAnim;

	/** is weapon fire active? */
	bool bWantsToFire;

	/** weapon is refiring */
	bool bRefiring;

	/** time of last successful weapon fire */
	float LastFireTime;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;

protected:
	
	/** weapon projectile fire implementation */
	UFUNCTION(BlueprintCallable)
	void FireWeapon();

	/** spawn projectile */
	UFUNCTION(BlueprintCallable)
    void FireProjectile(FVector Origin, FRotator ShootDir);

	/** handle weapon fire */
	UFUNCTION(BlueprintCallable)
	void HandleFiring();

	/** firing started */
	UFUNCTION(BlueprintCallable)
	virtual void OnBurstStarted();

	/** firing finished */
	UFUNCTION(BlueprintCallable)
	virtual void OnBurstFinished();

	/** update weapon state */
	UFUNCTION(BlueprintCallable)
	void SetWeaponState(EWeaponState NewState);

	/** determine current weapon state */
	UFUNCTION(BlueprintCallable)
	void DetermineWeaponState();

	/** The cosmetic effects for firing */
	void SimulateWeaponFire();

	/** Stop the cosmetic effects (e.g. for a looping shot). */
	void StopSimulatingWeaponFire();

public:

	/** check if weapon can fire */
	UFUNCTION(BlueprintCallable)
	bool CanFire() const;
	
	/** start weapon fire */
	UFUNCTION(BlueprintCallable)
	void StartFire();

	/** stop weapon fire */
	UFUNCTION(BlueprintCallable)
    void StopFire();

#pragma endregion Firing

	
//----------------------------------------------------------------------------------------------------------------------
// Ammo
//----------------------------------------------------------------------------------------------------------------------
#pragma region Ammo
protected:
	/** current total ammo */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category=Ammo)
	int32 CurrentAmmo;

	/** current ammo - inside clip */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category=Ammo)
    int32 CurrentAmmoInClip;

	/** out of ammo sound */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
    USoundCue* OutOfAmmoSound;

	/** pickup ammo sound */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	USoundCue* PickupAmmoSound;

public:
	/** add ammo */
	UFUNCTION(BlueprintCallable)
	bool GiveAmmo(int AddAmount, bool bNotify = false);

	/** consume a bullet */
	UFUNCTION(BlueprintCallable)
	void UseAmmo();

	/** checking ammo */
	UFUNCTION(BlueprintCallable)
	bool HasAmmo() const;

	/** check out of ammo in clip and inform about it */
	UFUNCTION(BlueprintCallable)
	void CheckOutOfAmmo();

	/** get current ammo amount (total) */
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmo() const;

	/** get current ammo amount (clip) */
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentAmmoInClip() const;

	/** get clip size */
	UFUNCTION(BlueprintCallable)
	int32 GetAmmoPerClip() const;

	/** get max ammo amount */
	UFUNCTION(BlueprintCallable)
	int32 GetMaxAmmo() const;

#pragma endregion Ammo
	
//----------------------------------------------------------------------------------------------------------------------
// Reload
//----------------------------------------------------------------------------------------------------------------------
#pragma region Reload
protected:
	
	/** is reloading? */
	bool bPendingReload;
	
	/** reload sound */
    UPROPERTY(EditDefaultsOnly, Category=Sound)
    USoundCue* ReloadSound;

	/** reload animations */
	UPROPERTY(EditDefaultsOnly, Category=Animation)
    UAnimMontage* ReloadAnim;

	/** Handle for efficient management of StartReload timer */
	FTimerHandle TimerHandle_ReloadWeapon;

	/** interrupt weapon reload */
	UFUNCTION(BlueprintCallable)
    void StopReload();

public:
	
	/** check if weapon can be reloaded */
	UFUNCTION(BlueprintCallable)
    bool CanReload() const;

	/** start weapon reload */
	UFUNCTION(BlueprintCallable)
    void StartReload();

#pragma endregion Reload
	
//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
#pragma region Inventory
protected:

	/** is weapon currently equipped? */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category=Inventory)
	bool bEquipped;

	/** equip animations */
	UPROPERTY(EditDefaultsOnly, Category=Animation)
    UAnimMontage* EquipAnim;

	/** equip sound */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
    USoundCue* EquipSound;

	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;

	/** attaches weapon mesh to pawn's mesh */
	UFUNCTION(BlueprintCallable)
    void AttachMeshToPawn() const;

	/** detaches weapon mesh from pawn */
	UFUNCTION(BlueprintCallable)
    void DetachMeshFromPawn() const;
	
public:

	/** weapon is being equipped by owner pawn */
	void OnEquip(const AWeapon* LastWeapon);

	/** weapon is now equipped by owner pawn */
	void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	void OnUnEquip();
	
	/** check if it's currently equipped */
	UFUNCTION(BlueprintCallable)
	bool IsEquipped() const;

	/** check if mesh is already attached */
	UFUNCTION(BlueprintCallable)
	bool IsAttachedToPawn() const;

	/** weapon was added to pawn's inventory */
	UFUNCTION(BlueprintCallable)
	void OnEnterInventory(ACharacter* NewOwner);

	/** weapon was removed from pawn's inventory */
	UFUNCTION(BlueprintCallable)
	void OnLeaveInventory();

#pragma endregion Inventory

//----------------------------------------------------------------------------------------------------------------------
// Weapon usage helpers
//----------------------------------------------------------------------------------------------------------------------
#pragma region Helpers
protected:
	
	/** play weapon sounds */
	UAudioComponent* PlayWeaponSound(USoundCue* Sound) const;

	/** play weapon animations */
	float PlayWeaponAnimation(UAnimMontage* Animation) const;

	/** stop playing weapon animations */
	void StopWeaponAnimation(UAnimMontage* Animation) const;

	/** Get the aim of the weapon, allowing for adjustments to be made by the weapon */
	virtual FVector GetAdjustedAim() const;

	/** Get the aim of the camera */
	FVector GetCameraAim() const;

	/** get the originating location for camera damage */
	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	/** get the muzzle location of the weapon */
	FVector GetMuzzleLocation() const;

	/** get direction of weapon's muzzle */
	FVector GetMuzzleDirection() const;

	/** find hit */
	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, bool bDebug = false) const;

#pragma endregion Helpers
};

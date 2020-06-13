// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"

#include "DrawDebugHelpers.h"
#include "Gynoid/Gynoid.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "WeaponComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectiles/Projectile.h"

AWeapon::AWeapon()
{
    MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
    MeshComp->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
    MeshComp->bReceivesDecals = false;

    WeaponConfig.AttachOffset = FTransform(FRotator(140.f, 25.f, 338.f), FVector(0.f, 3.f, 6.f));

    CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
}

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
}

//----------------------------------------------------------------------------------------------------------------------
// General
//----------------------------------------------------------------------------------------------------------------------

void AWeapon::SetWeaponOwner(ACharacter* NewOwner)
{
    if (WeaponOwner == NewOwner)
        return;

    SetInstigator(NewOwner);
    WeaponOwner = NewOwner;

    // Set Owner Weapon component
    TInlineComponentArray<UWeaponComponent*> WeaponComponents;
    WeaponOwner->GetComponents(WeaponComponents, false);
    OwnerComp = WeaponComponents.Pop();
}

//----------------------------------------------------------------------------------------------------------------------
// Firing
//----------------------------------------------------------------------------------------------------------------------
#pragma region Firing

void AWeapon::FireWeapon()
{
    FVector ShootDir = GetAdjustedAim();
    FVector Origin = GetMuzzleLocation();

    // trace from camera to check what's under crosshair
    const float ProjectileAdjustRange = 10000.0f;
    const FVector StartTrace = GetCameraDamageStartLocation(ShootDir);
    const FVector EndTrace = StartTrace + ShootDir * ProjectileAdjustRange;
    
    FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

    // and adjust directions to hit that actor
    if (Impact.bBlockingHit)
    {
        const FVector AdjustedDir = (Impact.ImpactPoint - Origin).GetSafeNormal();
        bool bWeaponPenetration = false;

        const float DirectionDot = FVector::DotProduct(AdjustedDir, ShootDir);
        if (DirectionDot < 0.0f)
        {
            // shooting backwards = weapon is penetrating
            bWeaponPenetration = true;
        }
        else if (DirectionDot < 0.5f)
        {
            // check for weapon penetration if angle difference is big enough
            // raycast along weapon mesh to check if there's blocking hit

            FVector MuzzleStartTrace = Origin - GetMuzzleDirection() * 150.0f;
            FVector MuzzleEndTrace = Origin;
            FHitResult MuzzleImpact = WeaponTrace(MuzzleStartTrace, MuzzleEndTrace);

            if (MuzzleImpact.bBlockingHit)
            {
                bWeaponPenetration = true;
            }
        }

        if (bWeaponPenetration)
        {
            // spawn at crosshair position
            Origin = Impact.ImpactPoint - ShootDir * 10.0f;
        }
        else
        {
            // adjust direction to hit

        }
    }

    FRotator ShootRotation = UKismetMathLibrary::FindLookAtRotation(Impact.ImpactPoint, EndTrace);

    FireProjectile(Origin, ShootRotation);
}

void AWeapon::FireProjectile(const FVector Origin, FRotator ShootDir)
{
   // DrawDebugLine(GetWorld(), Origin, ShootDir * 1000.f, FColor::Red, false, 5, 0.f, 1.f);

   const FTransform SpawnTransform(ShootDir, Origin);
    AProjectile* Projectile = Cast<AProjectile>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ProjectileConfig.ProjectileClass, SpawnTransform));
    if (Projectile)
    {
        Projectile->SetInstigator(GetInstigator());
        Projectile->SetOwner(this);
        //Projectile->InitVelocity(ShootDir);

        UGameplayStatics::FinishSpawningActor(Projectile, SpawnTransform);
    }
}

void AWeapon::HandleFiring()
{
    // try firing
    if (HasAmmo() && CanFire())
    {
        FireWeapon();

        UseAmmo();

        SimulateWeaponFire();
    } else
    {
        CheckOutOfAmmo();
        
        OnBurstFinished();
    }

    // try reload
    if (!HasAmmo() && CanReload())
    {
        StartReload();
    }

    // setup refire timer
    bRefiring = (CurrentState == EWeaponState::EWS_Firing && WeaponConfig.TimeBetweenShots > 0.f);
    if (bRefiring)
    {
        GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWeapon::HandleFiring, WeaponConfig.TimeBetweenShots, false);
    }

    LastFireTime = GetWorld()->GetTimeSeconds();
}

void AWeapon::OnBurstStarted()
{
    // start firing, can be delayed to satisfy TimeBetweenShots
    const float GameTime = GetWorld()->GetTimeSeconds();
    if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
        LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
    {
        GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
    }
    else
    {
        HandleFiring();
    }
}

void AWeapon::OnBurstFinished()
{

    GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
    bRefiring = false;
}

void AWeapon::SetWeaponState(EWeaponState NewState)
{
    const EWeaponState PrevState = CurrentState;

    if (PrevState == EWeaponState::EWS_Firing && NewState != EWeaponState::EWS_Firing)
    {
        OnBurstFinished();
    }

    CurrentState = NewState;

    if (PrevState != EWeaponState::EWS_Firing && NewState == EWeaponState::EWS_Firing)
    {
        OnBurstStarted();
    }
}

void AWeapon::DetermineWeaponState()
{
    EWeaponState NewState = EWeaponState::EWS_Idle;

    if (bEquipped)
    {
        // TODO: Implement reloading state

        if ((bWantsToFire == true) && (CanFire() == true))
        {
            NewState = EWeaponState::EWS_Firing;
        }

        // TODO: Implement equipping state
    }

    SetWeaponState(NewState);
}

void AWeapon::SimulateWeaponFire()
{
    if (CurrentState != EWeaponState::EWS_Firing)
        return;

    PlayWeaponSound(FireSound);
    // TODO: Implement looping sound
    
    // TODO: Implement Muzzle FX
    // TODO: Implement Animation
    // TODO: Implement Camera Shake   
}

void AWeapon::StopSimulatingWeaponFire()
{
    // TODO: Implement Stop Muzzle FX
    // TODO: Implement Stop Animation
    // TODO: Implement Stop sound

    // TODO: Add Fire Finish Sound
}

bool AWeapon::CanFire() const
{
    // TODO: Implement checking the Owner can fire (is Alive)
    // TODO: Implement checking for pending reload
    const bool bStateOKToFire = ((CurrentState == EWeaponState::EWS_Idle) || (CurrentState == EWeaponState::EWS_Firing));

    return bStateOKToFire;
}

void AWeapon::StartFire()
{
    if (!bWantsToFire)
    {
        bWantsToFire = true;
        DetermineWeaponState();
    }
}

void AWeapon::StopFire()
{
    if (bWantsToFire)
    {
        bWantsToFire = false;
        DetermineWeaponState();
    }
}

#pragma endregion Firing

//----------------------------------------------------------------------------------------------------------------------
// Ammo
//----------------------------------------------------------------------------------------------------------------------
#pragma region Ammo

void AWeapon::GiveAmmo(int AddAmount)
{
    const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
    AddAmount = FMath::Min(AddAmount, MissingAmmo);
    CurrentAmmo += AddAmount;

    // start reload if clip was empty
    if (GetCurrentAmmoInClip() <= 0 && CanReload() && OwnerComp && (OwnerComp->GetWeapon() == this))
    {
        StartReload();
    }
}

void AWeapon::UseAmmo()
{
    CurrentAmmoInClip--;
    CurrentAmmo--;
}

bool AWeapon::HasAmmo() const
{
    return CurrentAmmoInClip > 0;
}

void AWeapon::CheckOutOfAmmo()
{
    if (GetCurrentAmmo() == 0 && !bRefiring)
    {
        if (OutOfAmmoSound)
        {
            PlayWeaponSound(OutOfAmmoSound);
        }

        UE_LOG(LogTemp, Warning, TEXT("Out of Ammo!!!"));
        
        // TODO: HUD notification
    }
}

int32 AWeapon::GetCurrentAmmo() const
{
    return CurrentAmmo;
}

int32 AWeapon::GetCurrentAmmoInClip() const
{
    return CurrentAmmoInClip;
}

int32 AWeapon::GetAmmoPerClip() const
{
    return WeaponConfig.AmmoPerClip;
}

int32 AWeapon::GetMaxAmmo() const
{
    return WeaponConfig.MaxAmmo;
}


//----------------------------------------------------------------------------------------------------------------------
// Reload
//----------------------------------------------------------------------------------------------------------------------
#pragma region Reload

bool AWeapon::CanReload() const
{
    // TODO: Check is Character allows to reload

    const bool bGotAmmo = (CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0);
    const bool bStateOKToReload = (CurrentState == EWeaponState::EWS_Idle || CurrentState == EWeaponState::EWS_Firing);
    return bGotAmmo == true && bStateOKToReload;
}

void AWeapon::StartReload()
{
}

void AWeapon::StopReload()
{
}

void AWeapon::ReloadWeapon()
{
}

#pragma endregion Reload

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
#pragma region Inventory

void AWeapon::AttachMeshToPawn() const
{
    if (!WeaponOwner || !MeshComp)
        return;

    // Remove and hide attached meshes
    DetachMeshFromPawn();

    const FName AttachPoint = OwnerComp->GetWeaponAttachPoint();
    MeshComp->SetHiddenInGame(false);
    MeshComp->AttachToComponent(WeaponOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
    MeshComp->SetRelativeTransform(WeaponConfig.AttachOffset);
}

void AWeapon::DetachMeshFromPawn() const
{
    MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    MeshComp->SetHiddenInGame(true);
}

void AWeapon::OnEquip(const AWeapon* LastWeapon)
{
    bEquipped = true;
    AttachMeshToPawn();
    DetermineWeaponState();

    if (CurrentAmmoInClip <= 0 && CanReload())
    {
        StartReload();
    }
}

void AWeapon::OnEquipFinished()
{
    // TODO: Implement with animation
}

void AWeapon::OnUnEquip()
{
    bEquipped = false;
    DetachMeshFromPawn();
    StopFire();

    // TODO: Implement for pending reload

    // TODO: Implement for pending animation

    DetermineWeaponState();
}

bool AWeapon::IsEquipped() const
{
    return bEquipped;
}

bool AWeapon::IsAttachedToPawn() const
{
    return bEquipped; // TODO: Add pending animation
}

void AWeapon::OnEnterInventory(ACharacter* NewOwner)
{
    SetWeaponOwner(NewOwner);
}

void AWeapon::OnLeaveInventory()
{
    if (IsAttachedToPawn())
    {
        OnUnEquip();
    }

    SetWeaponOwner(nullptr);
}

#pragma endregion Inventory

//----------------------------------------------------------------------------------------------------------------------
// Weapon usage helpers
//----------------------------------------------------------------------------------------------------------------------
#pragma region Helpers

UAudioComponent* AWeapon::PlayWeaponSound(USoundCue* Sound) const
{
    UAudioComponent* AC = nullptr;
    if (Sound && WeaponOwner)
    {
        AC = UGameplayStatics::SpawnSoundAttached(Sound, WeaponOwner->GetRootComponent());
    }

    return AC;
}

float AWeapon::PlayWeaponAnimation(UAnimMontage* Animation) const
{
    float Duration = 0.0f;
    if (WeaponOwner && Animation)
    {
        Duration = WeaponOwner->PlayAnimMontage(Animation);
    }

    return Duration;
}

void AWeapon::StopWeaponAnimation(UAnimMontage* Animation) const
{
    if (WeaponOwner && Animation)
    {
        WeaponOwner->StopAnimMontage(Animation);
    }
}

FVector AWeapon::GetAdjustedAim() const
{
    APlayerController* const PlayerController = GetInstigatorController<APlayerController>();
    FVector FinalAim = FVector::ZeroVector;
    // If we have a player controller use it for the aim
    if (PlayerController)
    {
        FVector CamLoc;
        FRotator CamRot;
        PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
        FinalAim = CamRot.Vector();
    }
    else if (GetInstigator())
    {
        FinalAim = GetInstigator()->GetBaseAimRotation().Vector();
    }

    return FinalAim;
}

FVector AWeapon::GetCameraAim() const
{
    APlayerController* const PlayerController = GetInstigatorController<APlayerController>();
    FVector FinalAim = FVector::ZeroVector;

    if (PlayerController)
    {
        FVector CamLoc;
        FRotator CamRot;
        PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
        FinalAim = CamRot.Vector();
    }
    else if (GetInstigator())
    {
        FinalAim = GetInstigator()->GetBaseAimRotation().Vector();
    }

    return FinalAim;
}

FVector AWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
    APlayerController* PC = WeaponOwner ? Cast<APlayerController>(WeaponOwner->Controller) : nullptr;
    FVector OutStartTrace = FVector::ZeroVector;

    if (PC)
    {
        // use player's camera
        FRotator UnusedRot;
        PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);

        // Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
        OutStartTrace = OutStartTrace + AimDir * ((GetInstigator()->GetActorLocation() - OutStartTrace) | AimDir);
    }

    return OutStartTrace;
}

FVector AWeapon::GetMuzzleLocation() const
{
    return MeshComp->GetSocketLocation(MuzzleAttachPoint);
}

FVector AWeapon::GetMuzzleDirection() const
{
    return MeshComp->GetSocketRotation(MuzzleAttachPoint).Vector();
}

FHitResult AWeapon::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, const bool bDebug) const
{
    // Perform trace to retrieve hit info
    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
    TraceParams.bReturnPhysicalMaterial = true;
    TraceParams.bDebugQuery = true;

    FHitResult Hit(ForceInit);
    GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

    if (bDebug)
    {
        DrawDebugLine(GetWorld(), StartTrace, EndTrace, FColor::Green, false, 5, 0.f, 1.f);
    }

    return Hit;
}

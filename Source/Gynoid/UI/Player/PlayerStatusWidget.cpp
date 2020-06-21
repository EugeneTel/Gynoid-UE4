// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatusWidget.h"
#include "WeaponWidget.h"
#include "Gynoid/Gameplay/Weapons/Weapon.h"

void UPlayerStatusWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    // Subscribe to the Character Update Ammo Event
    AWeapon::NotifyUpdateAmmo.AddUObject(this, &UPlayerStatusWidget::OnUpdateAmmo);
}

void UPlayerStatusWidget::OnUpdateAmmo(int32& AmmoInClip, int32& CurrentAmmo) const
{   
    WeaponWidget->SetAmmo(AmmoInClip, CurrentAmmo);
}

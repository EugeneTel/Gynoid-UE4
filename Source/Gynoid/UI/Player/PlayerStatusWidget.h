// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "PlayerStatusWidget.generated.h"

class UWeaponWidget;

/**
 * 
 */
UCLASS()
class GYNOID_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

private:

    /** Weapon state widget set in WBP */
    UPROPERTY(VisibleAnywhere, meta = (BindWidget))
    UWeaponWidget* WeaponWidget;

protected:


    void NativePreConstruct() override;
public:
    /** On Character Update Ammo */
    void OnUpdateAmmo(int32& AmmoInClip, int32& CurrentAmmo) const;

    
};

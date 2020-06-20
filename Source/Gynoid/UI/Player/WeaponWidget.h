// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWidget.generated.h"

class UTextBlock;

/**
 * Weapon state representation
 */
UCLASS()
class GYNOID_API UWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	
	/** Ammo in clip and current ammo of player set in WBP */
	UPROPERTY(VisibleAnywhere, meta = (BindWidget))
	UTextBlock* AmmoTextBlock;
	
public:

	/** Set Ammo Text */
	void SetAmmo(int32& AmmoInClip, int32& CurrentAmmo) const;
	
};

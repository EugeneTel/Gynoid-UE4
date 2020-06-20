// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponWidget.h"
#include "Components/TextBlock.h"

void UWeaponWidget::SetAmmo(int32& AmmoInClip, int32& CurrentAmmo) const
{
   const int32 AmmoResource = FMath::Max(CurrentAmmo - AmmoInClip, 0);
   const FText AmmoText = FText::FromString(FString::Printf(TEXT("%d/%d"), AmmoInClip, AmmoResource));
   AmmoTextBlock->SetText(AmmoText);
}

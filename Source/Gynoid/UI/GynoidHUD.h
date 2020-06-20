// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Player/PlayerStatusWidget.h"

#include "GynoidHUD.generated.h"

/**
 * 
 */
UCLASS()
class GYNOID_API AGynoidHUD : public AHUD
{
	GENERATED_BODY()

private:

	/** Player Status Widget */
	UPROPERTY(VisibleInstanceOnly)
	UPlayerStatusWidget* PlayerStatusWidget;

	/** Player Status Widget Class */
	UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UPlayerStatusWidget> PlayerStateWidgetClass;

protected:

	/** Crosshair asset pointer */
	UTexture2D* CrosshairTexture;

	void PostInitializeComponents() override;

	/** Create Custom Widgets */
	void CreateCustomWidgets();

	/** Draws weapon crosshair. */
	void DrawCrosshair();

public:

	/** Main HUD update loop. */
	void DrawHUD() override;

	AGynoidHUD();

	
};

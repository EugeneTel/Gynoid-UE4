// Fill out your copyright notice in the Description page of Project Settings.


#include "GynoidHUD.h"
#include "Player/PlayerStatusWidget.h"

void AGynoidHUD::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    
    CreateCustomWidgets();
}

void AGynoidHUD::CreateCustomWidgets()
{
    // Create Player Status Widget
    if (!PlayerStatusWidget && PlayerStateWidgetClass)
    {
        PlayerStatusWidget = CreateWidget<UPlayerStatusWidget>(GetWorld(), PlayerStateWidgetClass);
        PlayerStatusWidget->AddToViewport();
    }
}

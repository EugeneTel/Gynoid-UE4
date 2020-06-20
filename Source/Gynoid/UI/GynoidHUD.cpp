// Fill out your copyright notice in the Description page of Project Settings.


#include "GynoidHUD.h"
#include "Player/PlayerStatusWidget.h"
#include "Engine/Canvas.h"

AGynoidHUD::AGynoidHUD()
{    
    // Set the crosshair texture
    static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/Gynoid/UI/Textures/T_Crosshair"));
    CrosshairTexture = CrosshairTexObj.Object;
}

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

void AGynoidHUD::DrawCrosshair()
{
    // Draw very simple crosshair

    // find center of the Canvas
    const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

    // offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
    const FVector2D CrosshairDrawPosition( (Center.X - 8.f), (Center.Y - 8.f));

    // draw the crosshair
    FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTexture->Resource, FLinearColor::White);
    TileItem.BlendMode = SE_BLEND_Translucent;
    Canvas->DrawItem( TileItem );
}

void AGynoidHUD::DrawHUD()
{
    Super::DrawHUD();
    
    DrawCrosshair();
}

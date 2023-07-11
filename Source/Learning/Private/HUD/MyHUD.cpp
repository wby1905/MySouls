// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MyHUD.h"
#include "HUD/MyOverlay.h"
void AMyHUD::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();
    if (World)
    {
        APlayerController* Controller = World->GetFirstPlayerController();
        if (Controller && MyOverlayClass)
        {
            MyOverlay = CreateWidget<UMyOverlay>(Controller, MyOverlayClass);
            MyOverlay->AddToViewport();
        }
    }
}

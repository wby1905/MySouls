// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/MyOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UMyOverlay::SetHealthPercent(float Percent)
{
    if (HealthProgressBar)
    {
        HealthProgressBar->SetPercent(Percent);
    }
}

void UMyOverlay::SetStaminaPercent(float Percent)
{
    if (StaminaProgressBar)
    {
        StaminaProgressBar->SetPercent(Percent);
    }
}

void UMyOverlay::SetGold(int32 Gold)
{
    if (GoldText)
    {
        const FString GoldString = FString::FromInt(Gold);
        GoldText->SetText(FText::FromString(GoldString));
    }
}

void UMyOverlay::SetSoul(int32 Soul)
{
    if (SoulText)
    {
        const FString SoulString = FString::FromInt(Soul);
        SoulText->SetText(FText::FromString(SoulString));
    }
}

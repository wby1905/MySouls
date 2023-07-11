// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyOverlay.generated.h"

class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class LEARNING_API UMyOverlay : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetHealthPercent(float Percent);
    void SetStaminaPercent(float Percent);
    void SetGold(int32 Gold);
    void SetSoul(int32 Soul);

private:
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UProgressBar> StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> GoldText;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> SoulText;


};

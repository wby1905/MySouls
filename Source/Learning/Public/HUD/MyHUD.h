// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyHUD.generated.h"

class UMyOverlay;

UCLASS()
class LEARNING_API AMyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	FORCEINLINE TObjectPtr<UMyOverlay> GetMyOverlay() const { return MyOverlay; }
protected:
    virtual void BeginPlay() override;
private:
	TObjectPtr<UMyOverlay> MyOverlay;

	UPROPERTY(EditDefaultsOnly, Category = MyHUD)
		TSubclassOf<UMyOverlay> MyOverlayClass;
};

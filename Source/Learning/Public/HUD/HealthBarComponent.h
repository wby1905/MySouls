// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class LEARNING_API UHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:

	void SetHealthPercent(float Percent);

	void FacePlayerCamera();
private:
	UPROPERTY()
	TObjectPtr<class UHealthBar> HealthBarWid;

};

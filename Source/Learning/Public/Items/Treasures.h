// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasures.generated.h"

/**
 * 
 */
UCLASS()
class LEARNING_API ATreasures : public AItem
{
	GENERATED_BODY()

public:
	FORCEINLINE int32 GetGold() const { return Gold; }

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
		int32 Gold;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

/**
 * 
 */
UCLASS()
class LEARNING_API ASoul : public AItem
{
	GENERATED_BODY()
public:
	FORCEINLINE int32 GetSouls() { return Souls; }
	FORCEINLINE void SetSouls(int32 Amount) { Souls = Amount; }
	
protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
		int32 Souls = 0;

};

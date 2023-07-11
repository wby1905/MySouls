// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Treasures.h"

#include "Interfaces/PickupInterface.h"

void ATreasures::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool FromSweep, const FHitResult& SweepResult)
{
	if (IPickupInterface* Pickup = Cast<IPickupInterface>(OtherActor))
	{
		PlayPickupSound();
		Pickup->AddGold(this);
		Destroy();
	}
}

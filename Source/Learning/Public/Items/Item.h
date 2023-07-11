// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped,
};

UCLASS()
class LEARNING_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		float RunningTime;
	
	UPROPERTY(VisibleAnywhere, BluePrintReadOnly)
		UStaticMeshComponent* ItemMesh;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
		TObjectPtr<class UNiagaraComponent> ItemEffect;

	UPROPERTY(EditAnywhere)
		TObjectPtr<class USoundBase> PickupSound;

	virtual void BeginPlay() override;

	virtual void SpawnPickupEffect();
	virtual void PlayPickupSound();


private:
	UPROPERTY(EditAnywhere)
		float Amplitude;

	UPROPERTY(EditAnywhere)
		float TimeConstant;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* PickupEffect;

	float TransformedSin();


};

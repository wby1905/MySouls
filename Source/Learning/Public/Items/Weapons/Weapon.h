// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class EWeaponKind : uint8
{
	EWK_OneHanded,
	EWK_TwoHanded,
};

/**
 * 
 */
UCLASS()
class LEARNING_API AWeapon : public AItem
{
	GENERATED_BODY()
	
public:
	TArray<AActor*> IgnoredActors;
	
	AWeapon();
	void Equip(TObjectPtr<USceneComponent> InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(TObjectPtr<USceneComponent> InParent, const FName& InSocketName);

	FORCEINLINE EWeaponKind GetWeaponKind() const { return WeaponKind; }
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponBox() const { return WeaponBox; }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);


	UFUNCTION(BlueprintImplementableEvent)
		void CreateFields(const FVector& FieldLocation);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		EWeaponKind WeaponKind;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		float Damage = 20.f;

	UPROPERTY(VisibleAnywhere, Category = "weapon Properties")
		TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USceneComponent> BoxTraceEnd;

	void DeactivateEmbers();
	void DisableSphereCollision();
	void PlayEquipSound();
	void ExecuteGetHit(FHitResult& BoxHit);
	void BoxTrace(FHitResult& BoxHit);
	bool ActorIsSameType(AActor* OtherActor) const;


};

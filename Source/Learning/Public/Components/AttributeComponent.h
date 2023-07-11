// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEARNING_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

	void ReceiveDamage(float Damage);
	void SpendStamina(float Amount);
	void RegenStamina(float DeltaTime);
	void AddGold(int32 Amount);
	void AddSouls(int32 Amount);

	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; }
	FORCEINLINE float GetStaminaPercent() const { return Stamina / MaxStamina; }
	FORCEINLINE bool IsAlive() const { return Health > 0.1f; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE int32 GetGold() const { return Gold; }
    FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetDodgeCost() const { return DodgeCost; }

private:
	// Current health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxHealth = 100.f;

	// Current Stamina
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float Stamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float MaxStamina = 100.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Gold;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		int32 Souls;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float DodgeCost = 20.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
		float StaminaRegenRate = 8.f;

};

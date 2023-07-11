// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterType.h"
#include "BaseCharacter.generated.h"

class AWeapon;

UCLASS()
class LEARNING_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

	FORCEINLINE EDeathPose GetDeathPose() const { return DeathPose; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = Combat)
		TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
		TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TObjectPtr<class UAttributeComponent> Attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
		TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
		double WarpTargetDistance = 75.f;

	UPROPERTY(BlueprintReadOnly)
		TEnumAsByte<EDeathPose> DeathPose;

	/** Combat */
	virtual void Attack();
	virtual bool CanAttack();
	virtual void Dodge();
	virtual bool CanDodge() const;

	UFUNCTION(BluePrintCallable)
		virtual void AttackEnd();

	UFUNCTION(BluePrintCallable)
        virtual void DodgeEnd();

	/** Get Hit */
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticle(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);

	UFUNCTION(BluePrintCallable)
		virtual void HitReactEnd();

	/** Montages */
	void PlayHitMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 SelectRandomMontageSection(UAnimMontage* Montage) const;
	void StopMontage(UAnimMontage* Montage);
	
	UFUNCTION(BluePrintCallable)
		FVector GetTranslationWarpTarget() const;

	UFUNCTION(BluePrintCallable)
		FVector GetRotationWarpTarget() const;


	/** Utils */
	void DisableCollision();
	void EnableCollision();
	virtual void Die();
	virtual bool IsAlive() const;

	UFUNCTION(BlueprintCallable)
		void ToggleWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

private:


	UPROPERTY(EditDefaultsOnly, Category = Combat)
		TObjectPtr<UAnimMontage> DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = Combat)
		TObjectPtr<UAnimMontage> HitMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
		TObjectPtr<USoundBase> HitSound;
	UPROPERTY(EditAnywhere, Category = Combat)
		TObjectPtr<UParticleSystem> HitParticle;

};

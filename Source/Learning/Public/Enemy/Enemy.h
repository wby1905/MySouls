// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterType.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"

class USoundBase;

UCLASS()
class LEARNING_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

	void ToggleHealthBar(bool bIsVisible);

	UFUNCTION(BlueprintImplementableEvent)
        void ToggleArrow(bool bIsVisible);

	FORCEINLINE EEnemyState GetEnemyState() const { return EnemyState; }

protected:
	UPROPERTY(BlueprintReadOnly)
		EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */
	

	/** <ABaseCharacter> */
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void Die() override;
	/** </ABaseCharacter> */

private:
	/**
	* Components
	*/
	
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<class UHealthBarComponent> HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<class UPawnSensingComponent> PawnSensing;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = Combat)
		TSubclassOf<class ASoul> SoulClass;

	UPROPERTY(EditAnywhere, Category = Combat)
		float DeathLifeSpan = 8.f;

	/**
	* Combat
	*/

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
		double CombatRadius = 750.f;
	UPROPERTY(EditAnywhere, Category = Combat)
		double AttackRadius = 150.f;
	UPROPERTY(EditAnywhere, Category = Combat)
		float AttackMin = 0.5f;
	UPROPERTY(EditAnywhere, Category = Combat)
		double AcceptanceRadius = 50.f;
	UPROPERTY(EditAnywhere, Category = Combat)
		float AttackMax = 1.5f;
	UPROPERTY(EditAnywhere, Category = Combat)
		float ChaseSpeed = 300.f;
	/**
	* Navigation
	*/
	TObjectPtr<class AAIController> EnemyController;


	// Patrol
	double PatrolRadius= 200.f;
	FTimerHandle PatrolTimer;
	UPROPERTY(EditAnywhere, Category = Combat)
		float PatrolSpeed = 125.f;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		TObjectPtr<AActor> PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
		TArray<TObjectPtr<AActor>> PatrolPoints;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
		float PatrolWaitMax = 10.f;

	/**
	* AI Behavior
	*/
	void InitEnemy();
	void PatrolTimerFinished();
	void CheckCombat();
	void CheckPatrol();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	void ClearPatrolTimer();
	void MoveToTarget(AActor* TargetActor);
	void SpawnDefaultWeapon();
	bool InTargetRange(AActor* TargetActor, double Radius);

	UFUNCTION()
		void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	/**
	* Combat
	*/
	void StartAttackTimer();
	void ClearAttackTimer();
	virtual void HandleDamage(float DamageAmount) override;
	void SpawnSouls();

};

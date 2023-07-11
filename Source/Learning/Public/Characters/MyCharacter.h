// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "CharacterType.h"
#include "BaseCharacter.h"
#include "Interfaces/PickupInterface.h"
#include "MyCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class UAnimMontage;
class AItem;
class AEnemy;



UCLASS()
class LEARNING_API AMyCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:

	AMyCharacter();
	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	/** </AActor> */

	/** <ACharacter> */
	virtual void Jump() override;
	/** </ACharacter> */


	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */

	/** <IPickupInterface> */
	virtual void SetOverlappingItem(AItem* Item) override { OverlappingItem = Item; } 
	virtual void AddSouls(class ASoul* Soul) override;
	virtual void AddGold(class ATreasures* Treasure) override;
	/** </IPickupInterface> */
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
protected:

	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/**
	* Play Montage functions
	*/
	void PlayEquipMontage(const FName& SectionName);

	UFUNCTION(BluePrintCallable)
		void AttachWeaponToHand();
	UFUNCTION(BluePrintCallable)
		void AttachWeaponToBack();
	UFUNCTION(BluePrintCallable)
		void FinishEquipping();

	/** Combat */

	/** <ABaseCharacter> */
	virtual void Die() override;
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	virtual void Dodge() override;
	virtual void DodgeEnd() override;
	virtual bool CanDodge() const override;
	virtual void HitReactEnd() override;
	/** </ABaseCharacter> */

private:
	/** Character Components */
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
		TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere, Category=Hair)
		TObjectPtr<UGroomComponent> Hair;

	UPROPERTY(VisibleAnywhere, Category=Hair)
		TObjectPtr<UGroomComponent> Eyebrows;

	UPROPERTY(VisibleInstanceOnly)
		TObjectPtr<AItem> OverlappingItem;

	UPROPERTY()
		TObjectPtr<class UMyOverlay> MyOverlay;

	/** Character States */
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BluePrintReadWrite, meta = (AllowPrivateAccess = "true"))
		EActionState ActionState = EActionState::EAS_Unoccupied;
	
	/** Enhanced Input Actions */
	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputMappingContext> MyCharContext;

	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, Category = "Input")
		TObjectPtr<UInputAction> EKeyAction;

	UPROPERTY(EditAnywhere, Category = "Input")
        TObjectPtr<UInputAction> TabAction;

	/** Montages */
	UPROPERTY(EditDefaultsOnly, Category = Montages)
		TObjectPtr<UAnimMontage> OneHandedMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
		TObjectPtr<UAnimMontage> TwoHandedMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
		TObjectPtr<UAnimMontage> EquipMontage;

	/** Align Camera */
	bool bIsRotating = false;
	double RotationTime = 0.0;
	UPROPERTY(EditAnywhere)
	double RotationDuration = 0.25f;
	FRotator TargetRotation = FRotator();

	/** Lock Enemy */
	bool bIsLocked = false;
	UPROPERTY(EditAnywhere)
	double LockRange = 1000.f;
	UPROPERTY(EditAnywhere)
	double LockSphereRadius = 100.f;
	TObjectPtr<AEnemy> TmpLockEnemy;

	/** init */
	void InitializeMyOverlay();
	void InitializeEnhancedInputMappings();

	/**
	*	Callbacks
	*/
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKey();
	void EquipWeapon();
	void Tab();

	/** Lock/Targeting */
	void UpdateLockEnemy();
	void LookAtEnemy();
	void UpdateTmpLockEnemy();
	FORCEINLINE void LockOnEnemy();
	FORCEINLINE void UnlockEnemy()
	{
        bIsLocked = false; 
        CombatTarget = nullptr; 
    }

	
	/** Align */
	void RotateCameraWithDeltaTime(float DeltaTime);
	void RotateCameraByAlpha(float Alpha);

	void StartAlignCamera();
	TObjectPtr<AEnemy> SphereHit();

	/** Combat */
	bool CanDisarm() const;
	bool CanArm() const;
	void DisArm();
	void Arm();
	void RegenStamina(float DeltaTime);

	/** HUD */
	void UpdateHealthHUD();

};

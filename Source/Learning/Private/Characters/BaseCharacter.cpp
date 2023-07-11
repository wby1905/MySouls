// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	
	ToggleWeaponCollision(ECollisionEnabled::NoCollision);
	
	
	if (IsAlive() && Hitter)
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else
	{
		Die();
	}
	PlayHitSound(ImpactPoint);
	SpawnHitParticle(ImpactPoint);

}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}
bool ABaseCharacter::CanAttack()
{
	return false;
}
void ABaseCharacter::Dodge()
{
}
bool ABaseCharacter::CanDodge() const
{
	return false;
}
void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::DodgeEnd()
{
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	if (GetWorld())
		{

			// get the angle between the forward vector and the impact vector
			const FVector Forward = GetActorForwardVector();
			const FVector ToImpact = (FVector(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z) - GetActorLocation()).GetSafeNormal();

			const float CosTheta = FVector::DotProduct(Forward, ToImpact);
			double Angle = FMath::RadiansToDegrees(FMath::Acos(CosTheta));

			// if cross product is negative, Theta should be negative
			const FVector CrossProduct = FVector::CrossProduct(Forward, ToImpact);
			if (CrossProduct.Z < 0.f)
			{
				Angle *= -1.f;
			}

			FName Section("FromBack");

			if (Angle <= 45.f && Angle > -45.f)
			{
				Section = FName("FromFront");
			}
			else if (Angle <= -45.f && Angle > -135.f)
			{
				Section = FName("FromLeft");
			}
			else if (Angle <= -135.f || Angle > 135.f)
			{
				Section = FName("FromBack");
			}
			else if (Angle <= 135.f && Angle > 45.f)
			{
				Section = FName("FromRight");
			}

			PlayHitMontage(Section);

		}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
}

void ABaseCharacter::SpawnHitParticle(const FVector& ImpactPoint)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactPoint);
	}
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::HitReactEnd()
{
}

void ABaseCharacter::DisableCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::EnableCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ABaseCharacter::Die()
{
	DisableCollision();
	PlayDeathMontage();
	GetCharacterMovement()->DisableMovement();
	SetCanBeDamaged(false);

	Tags.Add(FName("Dead"));
}

bool ABaseCharacter::IsAlive() const
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::PlayHitMontage(const FName& SectionName)
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitMontage)
	{
		AnimInstance->Montage_Play(HitMontage);

		AnimInstance->Montage_JumpToSection(SectionName, HitMontage);
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	if (!AttackMontage) return 0;
	const int32 Selection = SelectRandomMontageSection(AttackMontage);
	PlayMontageSection(AttackMontage, AttackMontage->GetSectionName(Selection));
	return Selection;
}

int32 ABaseCharacter::PlayDeathMontage()
{
	if (!DeathMontage) return 0;
	const int32 Selection = SelectRandomMontageSection(DeathMontage);

	PlayMontageSection
	(
		DeathMontage, 
		DeathMontage->GetSectionName(Selection)
	);

	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		DeathPose = Pose;
	}
	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::SelectRandomMontageSection(UAnimMontage* Montage) const
{
	const int32 MontageSections = Montage->GetNumSections();
	if (MontageSections <= 0) return -1;
	const int32 Selection = FMath::RandRange(0, MontageSections - 1);
	return Selection;
}

void ABaseCharacter::StopMontage(UAnimMontage* Montage)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
if (AnimInstance && Montage)
    {
        AnimInstance->Montage_Stop(0.25f, Montage);
    }
}

FVector ABaseCharacter::GetTranslationWarpTarget() const
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();
	const FVector TargetToActor =
		(Location - CombatTargetLocation).GetSafeNormal() * WarpTargetDistance;
	
	return CombatTargetLocation + TargetToActor;
}

FVector ABaseCharacter::GetRotationWarpTarget() const
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::ToggleWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{

	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoredActors.Empty();
	}
}




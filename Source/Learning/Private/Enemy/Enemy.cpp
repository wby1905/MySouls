// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Perception/PawnSensingComponent.h"

#include "HUD/HealthBarComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("EnemyHealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SetPeripheralVisionAngle(45.f);
	PawnSensing->SightRadius = 4000.f;

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (EnemyState == EEnemyState::EES_Dead) return;

	if (HealthBarWidget && HealthBarWidget->IsVisible())
	{
		HealthBarWidget->FacePlayerCamera();
	}

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombat();
	}
	else
	{
		CheckPatrol();
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
	if (InTargetRange(CombatTarget, AttackRadius))
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else
	{
		ChaseTarget();
	}
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	Super::Destroyed();
	if (EquippedWeapon)
	{
        EquippedWeapon->Destroy();
    }
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (EnemyState != EEnemyState::EES_Dead) ToggleHealthBar(true);
	ClearPatrolTimer();
	ClearAttackTimer();
	if (IsAlive())
	{
		EnemyState = EEnemyState::EES_NoState;
		CheckCombat();
	}
}

void AEnemy::ToggleHealthBar(bool bIsVisible)
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(bIsVisible);
	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddUniqueDynamic(this, &AEnemy::PawnSeen);
	}

	InitEnemy();

	Tags.Add(FName("EngageableTarget"));
	Tags.Add(FName("Enemy"));
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) return;
	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	return InTargetRange(CombatTarget, AttackRadius) &&
		EnemyState != EEnemyState::EES_Attacking &&
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Engaged;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombat();
}

void AEnemy::Die()
{

	EnemyState = EEnemyState::EES_Dead;
	Super::Die();

	ClearAttackTimer();
	ClearPatrolTimer();
	ToggleHealthBar(false);
	ToggleArrow(false);
	SetLifeSpan(DeathLifeSpan);
	SpawnSouls();
}

void AEnemy::InitEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	ToggleHealthBar(false);
	ToggleArrow(false);
	MoveToTarget(PatrolTarget);
	SpawnDefaultWeapon();
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::CheckCombat()
{
	// lose interest if it is too far away
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		ClearAttackTimer();
		LoseInterest();
		// if not swinging the weapon, go back to patrolling
		if (EnemyState != EEnemyState::EES_Engaged)
		{
			StartPatrolling();
		}
	} 
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		// Outside attack range, move to target
		ClearAttackTimer();
		if (EnemyState != EEnemyState::EES_Engaged)
		{
			ChaseTarget();
		}
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::CheckPatrol()
{
	// Patrol if no combat target and wait for patrol timer to go to next patrol point
	if (InTargetRange(PatrolTarget, PatrolRadius) && !PatrolPoints.IsEmpty())
	{
		const int32 NextIdx = (PatrolPoints.Find(PatrolTarget) + 1) % PatrolPoints.Num();
		PatrolTarget = PatrolPoints[NextIdx];
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, FMath::FRandRange(PatrolWaitMin, PatrolWaitMax));
	}
}

void AEnemy::LoseInterest()
{
	ToggleHealthBar(false);
	CombatTarget = nullptr;
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::MoveToTarget(AActor* TargetActor)
{
	if (EnemyController == nullptr || TargetActor == nullptr) return;
	FAIMoveRequest MoveRequest;
    MoveRequest.SetGoalActor(TargetActor);
    MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
    EnemyController->MoveTo(MoveRequest);
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("WeaponSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

bool AEnemy::InTargetRange(AActor* TargetActor, double Radius)
{
	if (TargetActor == nullptr) return false;
	const double Distance = FVector::Distance(GetActorLocation(), TargetActor->GetActorLocation());
	return Distance <= Radius;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	// if not in combat and not dead, chase the target (patrol, currently)
	const bool bShouldChaseTarget = 
		EnemyState != EEnemyState::EES_Dead && 
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking && 
		SeenPawn->ActorHasTag(FName("EngageableTarget"));
	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
	if (Attributes && HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::SpawnSouls()
{
	UWorld* World = GetWorld();
	if (World && SoulClass && Attributes)
	{
		ASoul* SpawnedSoul = World->SpawnActor<ASoul>(SoulClass, GetActorLocation(), GetActorRotation());
		if (SpawnedSoul)
		{
			SpawnedSoul->SetSouls(Attributes->GetSouls());
		}
	}
}



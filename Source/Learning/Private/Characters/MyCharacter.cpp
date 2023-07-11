// Fill out your copyright notice in the Description page of Project Settings.
# pragma once

#include "Characters/MyCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Enemy/Enemy.h"
#include "Items/Item.h"
#include "Items/Soul.h"
#include "Items/Treasures.h"
#include "Items/Weapons/Weapon.h"
#include "HUD/MyHUD.h"
#include "HUD/MyOverlay.h"
#include "Components/AttributeComponent.h"

AMyCharacter::AMyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    MovementComponent->bOrientRotationToMovement = true;
    MovementComponent->RotationRate = FRotator(0.0f, 300.0f, 0.0f);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 300.0f;

    ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
    ViewCamera->SetupAttachment(CameraBoom);

    ViewCamera->bUsePawnControlRotation = true;

    Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
    Hair->SetupAttachment(GetMesh());
    Hair->AttachmentName = FString("head");

    Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
    Eyebrows->SetupAttachment(GetMesh());
    Eyebrows->AttachmentName = FString("head");

    GetMesh()->SetCollisionObjectType(ECC_WorldDynamic);
    GetMesh()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
    GetMesh()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    GetMesh()->SetGenerateOverlapEvents(true);

}

void AMyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    RotateCameraWithDeltaTime(DeltaTime);
    UpdateLockEnemy();
    RegenStamina(DeltaTime);
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent * EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
    {
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMyCharacter::Jump);
        EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMyCharacter::Attack);
        EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AMyCharacter::Dodge);
        EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &AMyCharacter::EKey);
        EnhancedInputComponent->BindAction(TabAction, ETriggerEvent::Triggered, this, &AMyCharacter::Tab);

    }

}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (ActionState == EActionState::EAS_Dodging) return 0.f;
    HandleDamage(DamageAmount);
    UpdateHealthHUD();
    return DamageAmount;
}

void AMyCharacter::Jump()
{
    if (ActionState == EActionState::EAS_Unoccupied)
    {
        Super::Jump();
    }
}

void AMyCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
    if (ActionState == EActionState::EAS_Dodging) return;
    
    Super::GetHit_Implementation(ImpactPoint, Hitter);

    // interrupt any action
    if (ActionState == EActionState::EAS_Attacking)
    {
        StopMontage(AttackMontage);
    } 
    else if (ActionState == EActionState::EAS_EquippingWeapon)
    {
        StopMontage(EquipMontage);
        // Equip
        if (CharacterState == ECharacterState::ECS_Unequipped)
        {
            AttachWeaponToBack();
        }
        else 
        {
            AttachWeaponToHand();
        }
    }

    if (Attributes && Attributes->IsAlive())
    {
        ActionState = EActionState::EAS_HitReaction;
    }

}

void AMyCharacter::AddSouls(ASoul* Soul)
{
   if (Attributes && MyOverlay)
    {
        Attributes->AddSouls(Soul->GetSouls());
        MyOverlay->SetSoul(Attributes->GetSouls());
    }
}

void AMyCharacter::AddGold(ATreasures* Treasure)
{
    if (Attributes && MyOverlay)
    {
        Attributes->AddGold(Treasure->GetGold());
    MyOverlay->SetGold(Attributes->GetGold());
    }
}

void AMyCharacter::BeginPlay()
{
    Super::BeginPlay();   
    
    InitializeEnhancedInputMappings();
    InitializeMyOverlay();

    // Add the tag to the character for enemy to detect
    Tags.Add(FName("EngageableTarget"));
    Tags.Add(FName("Player"));

}

void AMyCharacter::PlayEquipMontage(const FName& SectionName)
{
    PlayMontageSection(EquipMontage, SectionName);
}

void AMyCharacter::AttachWeaponToHand()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHand_Socket"));
        EquipWeapon();
    }
}

void AMyCharacter::AttachWeaponToBack()
{
    if (EquippedWeapon)
    {
        EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
        CharacterState = ECharacterState::ECS_Unequipped;
    }
}

void AMyCharacter::FinishEquipping()
{
    ActionState = EActionState::EAS_Unoccupied;
}

void AMyCharacter::HitReactEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
}

void AMyCharacter::Die()
{
    ActionState = EActionState::EAS_Dead;
    Super::Die();
}

void AMyCharacter::Attack()
{
    if (CanAttack())
    {
        PlayAttackMontage();
        ActionState = EActionState::EAS_Attacking;
    }
}

void AMyCharacter::AttackEnd()
{
    ActionState = EActionState::EAS_Unoccupied;
}

bool AMyCharacter::CanAttack()
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState != ECharacterState::ECS_Unequipped;
}

bool AMyCharacter::CanDodge() const
{
    if (!Attributes) return false;
    return ActionState == EActionState::EAS_Unoccupied &&
        Attributes->GetStamina() >= Attributes->GetDodgeCost();
}

void AMyCharacter::InitializeMyOverlay()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        AMyHUD* MyHUD = Cast<AMyHUD>(PlayerController->GetHUD());
        if (MyHUD)
        {
            MyOverlay = MyHUD->GetMyOverlay();
            if (MyOverlay && Attributes)
            {
                MyOverlay->SetHealthPercent(Attributes->GetHealthPercent());
                MyOverlay->SetStaminaPercent(Attributes->GetStaminaPercent());
                MyOverlay->SetGold(Attributes->GetGold());
                MyOverlay->SetSoul(Attributes->GetSouls());
            }
        }
    }
}

void AMyCharacter::InitializeEnhancedInputMappings()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
    {
        // Add Enhanced Input Mapping Context
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(MyCharContext, 0);
        }
    }
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
    if (ActionState == EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_Dodging) {
        const FVector2D MoveVector = Value.Get<FVector2D>();
        const FRotator Rotation = GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(ForwardDirection, MoveVector.Y);

        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(RightDirection, MoveVector.X);
    }
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
    const FVector2D LookVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookVector.X);
    AddControllerPitchInput(LookVector.Y);

}

// Pickup/Equip/UnEquip the weapon
void AMyCharacter::EKey()
{
    TObjectPtr<AWeapon> OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
    if (OverlappingWeapon)
    {
        OverlappingWeapon->Equip(GetMesh(), FName("RightHand_Socket"), this, this);
        // destroy equipped weapon
        if (EquippedWeapon)
        {
            EquippedWeapon->Destroy();
        }
        EquippedWeapon = OverlappingWeapon;
        EquipWeapon();
        OverlappingItem = nullptr;
    }
    else 
    {
        // Unequip the weapon
        if (CanDisarm())
        {
            DisArm();
        }
        // Equip the weapon
        else if (CanArm())
        {
            Arm();
        }
    }
}

void AMyCharacter::EquipWeapon()
{
    if (EquippedWeapon->GetWeaponKind() == EWeaponKind::EWK_OneHanded)
    {
        CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
        AttackMontage = OneHandedMontage;
    }
    else if (EquippedWeapon->GetWeaponKind() == EWeaponKind::EWK_TwoHanded)
    {
        CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
        AttackMontage = TwoHandedMontage;
    }
}

// Align the camera to the character/ Lock(Unlock) the enemy
void AMyCharacter::Tab()
{
    if (!bIsLocked)
    {
        // if there is no enemy, then align the camera to the character's forward.
        if (TmpLockEnemy == nullptr) StartAlignCamera();
        else LockOnEnemy();
    }
    else {
        UnlockEnemy();
    }
}

void AMyCharacter::Dodge()
{
    if (CanDodge())
    {
        ActionState = EActionState::EAS_Dodging;
        Super::Dodge();
        PlayDodgeMontage();
        // use all stamina if there is not enough stamina.
        if (Attributes && MyOverlay)
        {
            Attributes->SpendStamina(Attributes->GetDodgeCost());
            MyOverlay->SetStaminaPercent(Attributes->GetStaminaPercent());
        }
    }
}

void AMyCharacter::DodgeEnd()
{
    Super::DodgeEnd();
    ActionState = EActionState::EAS_Unoccupied;
}

void AMyCharacter::UpdateLockEnemy()
{
    // If lock on one enemy, then look at the enemy
    if (bIsLocked)
    {
        AEnemy* Enemy = Cast<AEnemy>(CombatTarget);
        if (Enemy)
        {
            if (Enemy->GetEnemyState() == EEnemyState::EES_Dead)
            {
                // Unlock Enemy if Enemy is dead
                UnlockEnemy();
            }
            else
            {
                LookAtEnemy();
            }
        }
    }
    // else update the temporary locked enemy (camera's direction)
    else
    {
        UpdateTmpLockEnemy();
    }
}

void AMyCharacter::LookAtEnemy()
{
    if (!CombatTarget || !Controller) return;
    const FVector TargetLocation = CombatTarget->GetActorLocation();
    const FVector Direction = TargetLocation - GetActorLocation();
    const FRotator ControlRotation = Controller->GetControlRotation();
    const FRotator NewRotation(ControlRotation.Pitch, Direction.Rotation().Yaw, ControlRotation.Roll);
    Controller->SetControlRotation(NewRotation);
}

void AMyCharacter::UpdateTmpLockEnemy()
{
    AEnemy* Enemy = SphereHit();
    if (TmpLockEnemy)
    {
        TmpLockEnemy->ToggleArrow(false);
        TmpLockEnemy = nullptr;
    }
    if (Enemy && Enemy->GetEnemyState() != EEnemyState::EES_Dead)
    {
        Enemy->ToggleArrow(true);
        TmpLockEnemy = Enemy;
    }
}

void AMyCharacter::LockOnEnemy()
{
    bIsLocked = true;
    CombatTarget = TmpLockEnemy;
}

void AMyCharacter::RotateCameraWithDeltaTime(float DeltaTime)
{
    if (bIsRotating && !bIsLocked)
    {
        // Finish Rotating
        if (RotationTime > RotationDuration)
        {
            // Reset
            bIsRotating = false;
            RotationTime = 0.0f;
            RotateCameraByAlpha(1.0f);
        }
        else
        {
            RotationTime += DeltaTime;
            const float Alpha = RotationTime / RotationDuration;
            RotateCameraByAlpha(Alpha);
        }
    }
}

// Lerp the camera to the character's forward
void AMyCharacter::RotateCameraByAlpha(float Alpha)
{
    if (!Controller) return;
    const FRotator NewRotation = FMath::Lerp(Controller->GetControlRotation(), TargetRotation, Alpha);
    Controller->SetControlRotation(NewRotation);
}

void AMyCharacter::StartAlignCamera()
{
    FRotator ActorRotation = GetActorRotation();
    FRotator ControlRotation = GetControlRotation();
    bIsRotating = true;
    TargetRotation = FRotator(ControlRotation.Pitch, ActorRotation.Yaw, ControlRotation.Roll);
}

TObjectPtr<AEnemy> AMyCharacter::SphereHit()
{
    FVector Start = GetActorLocation();
    FRotator Rotation = GetControlRotation();
    FVector End = Start + FRotator(0, Rotation.Yaw, 0).Vector() * LockRange;

    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);

    // All Enemies are WorldDynamic
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));


    UKismetSystemLibrary::SphereTraceMultiForObjects
    (this, Start, End,
        LockSphereRadius,
        ObjectTypes,
        false,
        ActorsToIgnore,
        EDrawDebugTrace::None,
        HitResults,
        true
    );

    for (FHitResult HitResult : HitResults)
    {
        AEnemy* Enemy = Cast<AEnemy>(HitResult.GetActor());
        if (Enemy)
        {
            return Enemy;
        }
    }
    return nullptr;
}

bool AMyCharacter::CanDisarm() const
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState != ECharacterState::ECS_Unequipped;
}

bool AMyCharacter::CanArm() const
{
    return ActionState == EActionState::EAS_Unoccupied &&
        CharacterState == ECharacterState::ECS_Unequipped &&
        EquippedWeapon;
}

void AMyCharacter::DisArm()
{
    CharacterState = ECharacterState::ECS_Unequipped;
    ActionState = EActionState::EAS_EquippingWeapon;
    PlayEquipMontage(FName("Unequip"));
}

void AMyCharacter::Arm()
{
    PlayEquipMontage(FName("Equip"));
    EquipWeapon();
    ActionState = EActionState::EAS_EquippingWeapon;
}

void AMyCharacter::RegenStamina(float DeltaTime)
{
    if (Attributes && MyOverlay && ActionState != EActionState::EAS_Dodging)
    {
        Attributes->RegenStamina(DeltaTime);
        MyOverlay->SetStaminaPercent(Attributes->GetStaminaPercent());
    }
}

void AMyCharacter::UpdateHealthHUD()
{
    if (MyOverlay)
    {
        MyOverlay->SetHealthPercent(Attributes->GetHealthPercent());
    }
}
// Test Multiplayer Shooter Game. All Rights Reserved.


#include "MSBaseCharacter.h"

#include <Engine/Engine.h>
#include <Net/UnrealNetwork.h>
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"

#include "MSBaseWeapon.h"
#include "MSHealthComponent.h"
#include "MSWeaponComponent.h"
#include "MSCharacterMovementComponent.h"
#include "MSPlayerController.h"
#include "CommonInputDataAsset.h"
#include "WeaponInputDataAsset.h"
#include "GameFramework/SpringArmComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBaseCharacter, Log, All)

AMSBaseCharacter::AMSBaseCharacter(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UMSCharacterMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 86.0f);
    
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->bUsePawnControlRotation = true;
    
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent.Get());

	HealthComponent = CreateDefaultSubobject<UMSHealthComponent>("HealthComponent");
	HealthComponent->SetIsReplicated(true);

	HealthTextComponent = CreateDefaultSubobject<UTextRenderComponent>("HealthTextComponent");
	HealthTextComponent->SetupAttachment(GetRootComponent());
	HealthTextComponent->SetIsReplicated(true);

	WeaponComponent = CreateDefaultSubobject<UMSWeaponComponent>("WeaponComponent");
	WeaponComponent->SetIsReplicated(true);

	WeaponTextComponent = CreateDefaultSubobject<UTextRenderComponent>("WeaponTextComponent");
	WeaponTextComponent->SetupAttachment(GetRootComponent());
	WeaponTextComponent->SetIsReplicated(true);

	bReplicates = true;
}

void AMSBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//
	// DOREPLIFETIME(AMSBaseCharacter, HealthTextComponent);
	// DOREPLIFETIME(AMSBaseCharacter, HealthComponent);
	// DOREPLIFETIME(AMSBaseCharacter, WeaponComponent);
	// DOREPLIFETIME(AMSBaseCharacter, WeaponTextComponent);
}

void AMSBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AMSBaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// check(WeaponComponent);

	if (HasAuthority())
	{
		WeaponComponent->SpawnWeapon();
	}

	OnAmmoChanged();
	WeaponComponent->OnAmmoChanged.AddUObject(this, &AMSBaseCharacter::OnAmmoChanged);
    
	// check(HealthComponent)

    if (HealthComponent)
    {
        OnHealthChanged();
        HealthComponent->OnDeath.AddUObject(this, &AMSBaseCharacter::OnDeath);
        HealthComponent->OnHealthChanged.AddUObject(this, &AMSBaseCharacter::OnHealthChanged);
    }
}

// TODO: rework for enhanced input
void AMSBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    const TObjectPtr<AMSPlayerController> playerController = GetController<AMSPlayerController>();
    if (!IsValid(playerController.Get()))
    {
        UE_LOG(LogBaseCharacter, Error, TEXT("SetupPlayerInputComponent: Invalid PlayerController"));
        return;
    }
    
	const TObjectPtr<UEnhancedInputLocalPlayerSubsystem> EInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());
    const TObjectPtr<UEnhancedInputComponent> EInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
    if (!IsValid(EInputSubsystem.Get()) || !IsValid(EInputComponent.Get()))
    {
        UE_LOG(LogBaseCharacter, Error, TEXT("SetupPlayerInputComponent: Invalid EnhancedInputSubsystem or EnhancedInputComponent"));
        return;
    }

    if (CommonInputDataAsset && CommonInputMapping)
    {
        EInputSubsystem->AddMappingContext(CommonInputMapping.Get(), 0);

        if (CommonInputDataAsset->IA_Look)
        {
            EInputComponent->BindAction(CommonInputDataAsset->IA_Look.Get(), ETriggerEvent::Triggered, this, &AMSBaseCharacter::Look);
        }
        
        if (CommonInputDataAsset->IA_Move)
        {
            EInputComponent->BindAction(CommonInputDataAsset->IA_Move.Get(), ETriggerEvent::Triggered, this, &AMSBaseCharacter::Move);
        }

        if (CommonInputDataAsset->IA_Jump)
        {
            EInputComponent->BindAction(CommonInputDataAsset->IA_Jump.Get(), ETriggerEvent::Triggered, this, &AMSBaseCharacter::Jump);
        }

        if (CommonInputDataAsset->IA_Walk)
        {
            EInputComponent->BindAction(CommonInputDataAsset->IA_Walk.Get(), ETriggerEvent::Triggered, this, &AMSBaseCharacter::Walk, true);
            EInputComponent->BindAction(CommonInputDataAsset->IA_Walk.Get(), ETriggerEvent::Completed, this, &AMSBaseCharacter::Walk, false);
        }

        if (CommonInputDataAsset->IA_Crouch)
        {
            EInputComponent->BindAction(CommonInputDataAsset->IA_Crouch.Get(), ETriggerEvent::Triggered, this, &AMSBaseCharacter::Crouch, true);
            EInputComponent->BindAction(CommonInputDataAsset->IA_Crouch.Get(), ETriggerEvent::Completed, this, &AMSBaseCharacter::Crouch, true);
        }
    }
    else
    {
        UE_LOG(LogBaseCharacter, Error, TEXT("SetupPlayerInputComponent: Invalid CommonInputDataAsset or CommonInputMapping"));
    }

    if (WeaponInputDataAsset && WeaponInputMapping)
    {
        EInputSubsystem->AddMappingContext(WeaponInputMapping.Get(), 1);

        if (WeaponInputDataAsset->IA_Fire)
        {
            EInputComponent->BindAction(WeaponInputDataAsset->IA_Fire.Get(), ETriggerEvent::Triggered, this, &AMSBaseCharacter::StartFire);
            EInputComponent->BindAction(WeaponInputDataAsset->IA_Fire.Get(), ETriggerEvent::Completed, this, &AMSBaseCharacter::StopFire);
        }

        if (WeaponInputDataAsset->IA_Aim)
        {
            EInputComponent->BindAction(WeaponInputDataAsset->IA_Aim.Get(), ETriggerEvent::Triggered, this, &AMSBaseCharacter::Aim, true);
            EInputComponent->BindAction(WeaponInputDataAsset->IA_Aim.Get(), ETriggerEvent::Completed, this, &AMSBaseCharacter::Aim, false);
        }

        if (WeaponInputDataAsset->IA_Reload)
        {
            EInputComponent->BindAction(WeaponInputDataAsset->IA_Reload.Get(), ETriggerEvent::Triggered, this, &AMSBaseCharacter::Reload);
        }
    }
    else
    {
        UE_LOG(LogBaseCharacter, Error, TEXT("SetupPlayerInputComponent: Invalid WeaponInputDataAsset or WeaponInputMapping"));
    }
}

void AMSBaseCharacter::Look(const FInputActionValue& InputActionValue)
{
    if (!Controller) { return; }

    const FVector2D LookValue = InputActionValue.Get<FVector2D>();
    
    if (LookValue.X != 0.0f)
    {
        AddControllerYawInput(LookValue.X);
    }

    if (LookValue.Y != 0.0f)
    {
        // or in BP_PlayerController
        AddControllerPitchInput(-LookValue.Y);
    }
}

void AMSBaseCharacter::Move(const FInputActionValue& InputActionValue)
{
    if (!Controller) { return; }
    
    const FVector2D MoveValue = InputActionValue.Get<FVector2D>();
    const FRotator MoveRotation(0, Controller->GetControlRotation().Yaw, 0);

    // forward/backward direction
    if (MoveValue.Y != 0.0f)
    {
        const FVector Direction = MoveRotation.RotateVector(FVector::ForwardVector);
        AddMovementInput(Direction, MoveValue.Y);
    }

    if (MoveValue.X != 0.0f)
    {
        const FVector Direction = MoveRotation.RotateVector(FVector::RightVector);
        AddMovementInput(Direction, MoveValue.X);
    }
}

void AMSBaseCharacter::Walk(const FInputActionValue& InputActionValue, bool bIntendToWalk)
{
}

void AMSBaseCharacter::StartFire(const FInputActionValue& InputActionValue)
{
}

void AMSBaseCharacter::StopFire(const FInputActionValue& InputActionValue)
{
}

void AMSBaseCharacter::Aim(const FInputActionValue& InputActionValue, bool bIntendToAim)
{
}

void AMSBaseCharacter::Reload(const FInputActionValue& InputActionValue)
{
}

void AMSBaseCharacter::OnRep_HealthChanged()
{
    OnHealthChanged();
}

void AMSBaseCharacter::OnRep_AmmoChanged()
{
    OnAmmoChanged();
}

// void AMSBaseCharacter::StartWalk()
// {
// 	//auto Movement = GetCharacterMovement();
// 	//Movement->MaxWalkSpeed /= 2;
//
// 	bIsWalking = true;
// }
//
// void AMSBaseCharacter::StopWalk()
// {
// 	//auto Movement = GetCharacterMovement();
// 	//Movement->MaxWalkSpeed *= 2;
//
// 	bIsWalking = false;
// }

bool AMSBaseCharacter::IsWalking() const
{
	return bIsWalking && !GetVelocity().IsZero();
}

float AMSBaseCharacter::GetMovementDirection() const
{
	if (GetVelocity().IsZero())
		return 0.0f;

	const auto VelocityNormal = GetVelocity().GetSafeNormal();
	const auto ForwardNormal = GetActorForwardVector();

	const auto Angle = FMath::Acos(FVector::DotProduct(VelocityNormal, ForwardNormal));
	const auto DirectionCoef = FMath::Sign(FVector::CrossProduct(ForwardNormal, VelocityNormal).Z);

	return FMath::RadiansToDegrees(Angle) * DirectionCoef;
}

// void AMSBaseCharacter::MoveForward(float Amount)
// {
// 	if (Amount == 0.0f) { return; }
//
// 	AddMovementInput(GetActorForwardVector(), Amount);
// }
//
// void AMSBaseCharacter::MoveRight(float Amount)
// {
// 	if (Amount == 0.0f) { return; }
//
//     AddMovementInput(GetActorRightVector(), Amount);
// }

void AMSBaseCharacter::OnDeath()
{
	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(1.5f);
	WeaponComponent->GetWeapon()->SetLifeSpan(1.5f);

	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}

	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
}

// TODO: rework
void AMSBaseCharacter::OnHealthChanged()
{
    if (HealthComponent && HealthTextComponent)
    {
        if (HealthComponent->GetHealth() > 0)
        {
            const float colorCoef = HealthComponent->GetHealth() / HealthComponent->GetMaxHealth();
            const FLinearColor Color = FLinearColor::Red * (1 - colorCoef) + FLinearColor::Green * colorCoef;

            HealthTextComponent->SetTextRenderColor(Color.ToFColor(true));
            HealthTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), HealthComponent->GetHealth())));
        }
        else
        {
            HealthTextComponent->SetText(FText::FromString(TEXT("DEAD")));
            HealthTextComponent->SetTextRenderColor(FColor::Red);
            HealthTextComponent->SetOnlyOwnerSee(false);

            OnDeath();
        }
    }
}

void AMSBaseCharacter::OnAmmoChanged() const
{
    if (WeaponComponent && WeaponTextComponent)
    {
        const int32 CurrentAmmo = WeaponComponent->GetCurrentAmmo();
        const int32 AllAmmo = WeaponComponent->GetAllAmmo();

        WeaponTextComponent->SetTextRenderColor(FColor::Orange);
        WeaponTextComponent->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrentAmmo, AllAmmo)));
    }
}

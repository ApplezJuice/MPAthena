// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Athena/HUD/RemoteWidgetComponent.h"
#include "Components/ChildActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Athena/Weapon/Weapon.h"
#include "Athena/AthenaComponents/CombatComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<URemoteWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	// Replicated components don't need to be registered in GetLifetimeReplicatedProps
	Combat->SetIsReplicated(true);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlayerCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (InputMapping)
			{
				// Add IMC
				Subsystem->AddMappingContext(InputMapping, 0);
			}
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			UE_LOG(LogTemp, Display, TEXT("Binding MoveAction"));
			Input->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
		}
		else 
		{
			UE_LOG(LogTemp, Error, TEXT("MoveAction is null"));
		}
		Input->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
		Input->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
		Input->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::CrouchPressed);
		Input->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ThisClass::CrouchPressed);
		Input->BindAction(InteractAction, ETriggerEvent::Started, this, &ThisClass::Interact);

		Input->BindAction(AimAction, ETriggerEvent::Started, this, &ThisClass::AimPressed);
		Input->BindAction(AimAction, ETriggerEvent::Completed, this, &ThisClass::AimReleased);
	}
}

void APlayerCharacter::Move(const FInputActionInstance& Instance)
{
	FVector2D dir = Instance.GetValue().Get<FVector2D>();
	const FRotator rot(0.f, Controller->GetControlRotation().Yaw, Controller->GetControlRotation().Roll);
	const FVector rightDir(FRotationMatrix(rot).GetUnitAxis(EAxis::Y));
	const FVector forwardDir(FRotationMatrix(rot).GetUnitAxis(EAxis::X));
	AddMovementInput(rightDir, dir.X);
	AddMovementInput(forwardDir, dir.Y);
}

void APlayerCharacter::Look(const FInputActionInstance& Instance)
{
	FVector2D lookDir = Instance.GetValue().Get<FVector2D>();
	AddControllerYawInput(lookDir.X);
	AddControllerPitchInput(lookDir.Y);
}

void APlayerCharacter::Jump(const FInputActionInstance& Instance)
{
	Super::Jump();
}

void APlayerCharacter::CrouchPressed(const FInputActionInstance& Instance)
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else 
	{
		Crouch();
	}
}

void APlayerCharacter::Interact(const FInputActionInstance& Instance)
{
	if (OverlappingWeapon)
	{
		if (Combat)
		{
			if (HasAuthority())
			{
				Combat->EquipWeapon(OverlappingWeapon);
			}
			else
			{
				// Calling from the client, call the RPC
				ServerInteractButtonPressed();
			}
		}
	}
}

void APlayerCharacter::AimPressed(const FInputActionInstance& Instance)
{
	if (Combat)
	{
		Combat->SetAiming(true);
	}
}

void APlayerCharacter::AimReleased(const FInputActionInstance& Instance)
{
	if (Combat)
	{
		Combat->SetAiming(false);
	}
}

void APlayerCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void APlayerCharacter::ServerInteractButtonPressed_Implementation()
{
	if (OverlappingWeapon)
	{
		if (Combat)
		{
			Combat->EquipWeapon(OverlappingWeapon);
		}
	}
}

void APlayerCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}

	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

bool APlayerCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool APlayerCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}


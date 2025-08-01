// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "EnemyCharacter.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	_ShotStationaryTimer = 0.f;
	_Health = _MaxHealth;
}

APlayerCharacter::APlayerCharacter(bool usePlayerInput) : APlayerCharacter()
{
	UsePlayerInput = usePlayerInput;
}

void APlayerCharacter::TakeDamage(int damage)
{
	_Health -= damage;
	if (_Health <= 0)
	{
		Destroy();
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		playerController->SetShowMouseCursor(true);
		if (UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subSystem->AddMappingContext(_InputContext, 0);
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	const FVector2D movementVector = value.Get<FVector2D>();
	MoveAct(movementVector);
}

void APlayerCharacter::MoveAct(FVector2D movementVector)
{
	FRotator rotation = Controller->GetControlRotation();
	FRotator yawRotation(0.f, rotation.Yaw, 0.f);

	AddMovementInput(FVector(1.f, 0.f, 0.f), movementVector.Y);
	AddMovementInput(FVector(0.f, 1.f, 0.f), movementVector.X);
}

void APlayerCharacter::Shoot()
{
	ShootAct(GetActorLocation(), GetActorForwardVector());
}

void APlayerCharacter::ShootAct(FVector pos, FVector direction)
{
	FVector Start = GetActorLocation();
	FVector End = Start + (direction * 10000);
	_ShotDirection = End - Start;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		if (AEnemyCharacter* hitEnemy = Cast<AEnemyCharacter>(HitResult.GetActor()))
		{
			hitEnemy->TakeDamage();
		}
	}

	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		DisableInput(playerController);
	}
	_ShotStationaryTimer = ShotStationaryTime;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (_ShotStationaryTimer > 0.f)
	{
		_ShotStationaryTimer -= DeltaTime;

		if (_ShotStationaryTimer <= 0.f)
		{
			if (APlayerController* playerController = Cast<APlayerController>(Controller))
			{
				EnableInput(playerController);
			}
		}
		else
		{
			SetActorRotation(_ShotDirection.Rotation());
			return;
		}
	}

	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		FVector mouseWorldPosition, mouseWorldDirection;
		playerController->DeprojectMousePositionToWorld(mouseWorldPosition, mouseWorldDirection);

		FVector Start = mouseWorldPosition;
		FVector End = Start + (mouseWorldDirection * 10000);

		FHitResult HitResult;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
		{
			FVector hitLocation = HitResult.Location;

			FVector playerPos = GetActorLocation();
			FVector directionToMouse = hitLocation - playerPos;
			directionToMouse.Z = 0;
			directionToMouse.Normalize();

			if (!directionToMouse.IsZero())
			{
				FRotator newRotation = directionToMouse.Rotation();
				SetActorRotation(newRotation);
			}
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (!UsePlayerInput)
		return;

	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		enhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Shoot);
	}
}

void APlayerCharacter::SimulateFrame(const PlayerFrameRecording& frame)
{
	Tick(frame.DeltaTime);

	MoveAct(frame.MovementInput);
	SetActorRotation(frame.ForwardVector.Rotation());

	if (frame.ShootInput)
	{
		ShootAct(frame.ShotPosition, frame.ShotDirection);
	}
}


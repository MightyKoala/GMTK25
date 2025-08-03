// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "EnemyCharacter.h"
#include "Components/InputComponent.h"
#include "DefaultGameMode.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "DefaultGameInstance.h"
#include "Camera/CameraActor.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	IsPlayer = true;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArm->SetupAttachment(RootComponent);

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	PlayerCamera->SetupAttachment(CameraSpringArm);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		playerController->SetShowMouseCursor(true);
		playerController->SetAudioListenerOverride(GetMesh(), GetActorLocation(), GetActorRotation());
		if (UEnhancedInputLocalPlayerSubsystem* subSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
		{
			subSystem->AddMappingContext(_InputContext, 0);
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	MoveAct(value.Get<FVector2D>());
}

void APlayerCharacter::Shoot()
{
	if (!IsAlive)
		return;
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode && GameMode->IsLevelOver())
	{
		return;
	}
	if (FireRateTimer <= 0.f)
		_CurrentFrame.ShootInput = true;
	OnShootEvent();
}

void APlayerCharacter::FastForwardTime()
{
	TargetTimeDilation = 3.f;
}

void APlayerCharacter::StopFastForward()
{
	TargetTimeDilation = 1.f;
}

void APlayerCharacter::UpdateTimeDilation(float DeltaTime)
{
	FastforwardLerpValue = FMath::FInterpTo(FastforwardLerpValue, TargetTimeDilation, DeltaTime, TimeDilationSpeed);

	UGameplayStatics::SetGlobalTimeDilation(this, FastforwardLerpValue);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTimeDilation(DeltaTime);

	if (!IsAlive)
		return;

	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	
	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		if (GameMode && GameMode->IsLevelOver())
		{
			DisableInput(playerController);
			return;
		}

		float MouseX, MouseY;
		playerController->GetMousePosition(MouseX, MouseY);

		FVector WorldLocation, WorldDirection;
		playerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

		float ZDifference = GetActorLocation().Z - WorldLocation.Z;
		float Scale = ZDifference / WorldDirection.Z;

		FVector TargetPosition = WorldLocation + (WorldDirection * Scale);

		FVector playerPos = GetActorLocation();
		FVector directionToMouse = TargetPosition - playerPos;
		directionToMouse.Z = 0.0f;
		FRotator newRotation = directionToMouse.Rotation();
		SetActorRotation(newRotation);
	}

	_CurrentFrame.Location = GetActorLocation();
	_CurrentFrame.ForwardVector = GetActorForwardVector();

	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameMode && GameInstance && GameMode->GetLevelTimer() > 0.f)
	{
		_CurrentFrame.TimeStamp = GameMode->GetLevelMaxTime() - GameMode->GetLevelTimer();
		GameInstance->RecordFrame(_CurrentFrame);
	}
	_CurrentFrame.Reset();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* enhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		enhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		enhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Shoot);
		enhancedInputComponent->BindAction(FFAction, ETriggerEvent::Triggered, this, &APlayerCharacter::FastForwardTime);
		enhancedInputComponent->BindAction(FFAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopFastForward);
	}
}


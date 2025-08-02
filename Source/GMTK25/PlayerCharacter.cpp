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
	UE_LOG(LogTemp, Warning, TEXT("Shot Logged"));
	if (FireRateTimer <= 0.f)
		_CurrentFrame.ShootInput = true;
	OnShootEvent();
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsAlive)
		return;

	_CurrentFrame.Location = GetActorLocation();
	_CurrentFrame.ForwardVector = GetActorForwardVector();
	
	if (FireRateTimer <= 0.f)
	{
		if (APlayerController* playerController = Cast<APlayerController>(Controller))
		{
			float MouseX, MouseY;
			playerController->GetMousePosition(MouseX, MouseY);

			FVector WorldLocation, WorldDirection;
			playerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

			float ZDifference = GetActorLocation().Z - WorldLocation.Z;
			float Scale = ZDifference / WorldDirection.Z;

			FVector TargetPosition = WorldLocation + (WorldDirection * Scale);

			DrawDebugLine(GetWorld(), WorldLocation, TargetPosition, FColor::Red);
			DrawDebugSphere(GetWorld(), TargetPosition, 100.f, 12, FColor::Red);
			//UE_LOG(LogTemp, Warning, TEXT("Camera component Forward: X=%f, Y=%f, Z=%f"), ForwardVector.X, ForwardVector.Y, ForwardVector.Z);

			FVector playerPos = GetActorLocation();
			FVector directionToMouse = TargetPosition - playerPos;
			directionToMouse.Z = 0.0f;
			FRotator newRotation = directionToMouse.Rotation();
			SetActorRotation(newRotation);
			_CurrentFrame.ForwardVector = GetActorForwardVector();
		}
	}

	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
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
	}
}


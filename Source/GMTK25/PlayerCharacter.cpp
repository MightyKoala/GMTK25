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
	OnShootEvent();
	_CurrentFrame.ShootInput = true;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	_CurrentFrame.Location = GetActorLocation();
	_CurrentFrame.ForwardVector = GetActorForwardVector();
	
	if (FireRateTimer <= 0.f)
	{
		if (APlayerController* playerController = Cast<APlayerController>(Controller))
		{
			/*FVector mouseWorldPosition, mouseWorldDirection;
			playerController->DeprojectMousePositionToWorld(mouseWorldPosition, mouseWorldDirection);
			auto mousePos = playerController->GetMousePosition();
			playerController->DeprojectScreenPositionToWorld(mousePos.X, mousePos.Y, mouseWorldPosition, mouseWorldDirection)

			float AngleRadians = FMath::Atan2(mouseWorldDirection.Y, mouseWorldDirection.X);

			float SinValue = FMath::Sin(AngleRadians);

			if (SinValue == 0)
			{
				UE_LOG(LogTemp, Error, TEXT("PlayerCharacter.h | SinValue 0 on mouse rotator"));
				return;
			}

			float Distance = mouseWorldPosition.Z / SinValue;

			FVector IntersectionPoint = mouseWorldPosition + (mouseWorldDirection * Distance);
			IntersectionPoint.Z = 0.0f;*/

			float MouseX, MouseY;
			playerController->GetMousePosition(MouseX, MouseY);

			FVector WorldLocation, WorldDirection;
			playerController->DeprojectScreenPositionToWorld(MouseX, MouseY, WorldLocation, WorldDirection);

			FVector ForwardVector = PlayerCamera->GetForwardVector();

			ForwardVector = FRotator(0.f, 180.f, 0.f).RotateVector(ForwardVector);

			ForwardVector = WorldLocation - PlayerCamera->GetComponentLocation();
			ForwardVector.Normalize();

			// Calculate the target position at the specified height
			// We need to find the intersection of the forward vector with the height plane
			float ZDifference = 0.5f - WorldLocation.Z;
			float Scale = ZDifference / ForwardVector.Z;

			FVector TargetPosition = WorldLocation + (ForwardVector * Scale);

			DrawDebugLine(GetWorld(), WorldLocation, TargetPosition, FColor::Red);
			DrawDebugSphere(GetWorld(), TargetPosition, 100.f, 12, FColor::Red);
			//UE_LOG(LogTemp, Warning, TEXT("Camera component Forward: X=%f, Y=%f, Z=%f"), mouseWorldDirection.X, mouseWorldDirection.Y, mouseWorldDirection.Z);

			FVector playerPos = GetActorLocation();
			playerPos.Z = 0.0f;
			FVector directionToMouse = TargetPosition - playerPos;
			FRotator newRotation = directionToMouse.Rotation();
			SetActorRotation(newRotation);
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


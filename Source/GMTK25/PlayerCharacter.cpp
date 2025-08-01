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

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	IsPlayer = true;
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
	ShootAct(GetActorLocation(), GetActorForwardVector());
	UE_LOG(LogTemp, Warning, TEXT("Shot Logged"));
	_CurrentFrame.ShootInput = true;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	_CurrentFrame.Location = GetActorLocation();
	_CurrentFrame.ForwardVector = GetActorForwardVector();
	
	if (_ShotStationaryTimer <= 0.f)
	{
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


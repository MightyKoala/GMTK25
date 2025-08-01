// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "EnemyCharacter.h"
#include "DefaultGameMode.h"
#include "DefaultGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	_ShotStationaryTimer = 0.f;
	_Health = _MaxHealth;
}

void ACharacterBase::DamageCharacter(int damage)
{
	_Health -= damage;
	if (_Health <= 0)
	{
		if(IsPlayer)
			OnPlayerDeath();
		Destroy();
	}
}

void ACharacterBase::OnPlayerDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("OnPlayerDeath called"));
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		GameInstance->IncreaseDeathCount();
		GameInstance->StoreRecordedFrames();

		ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			UE_LOG(LogTemp, Warning, TEXT("Resetting gamemode, lives left: %d"), GameInstance->GetDeathCount());
			GameMode->ReloadLevel();
		}
	}
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::Tick(float DeltaTime)
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
}

void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACharacterBase::MoveAct(FVector2D movementVector)
{
	FRotator rotation = Controller->GetControlRotation();
	FRotator yawRotation(0.f, rotation.Yaw, 0.f);

	AddMovementInput(FVector(1.f, 0.f, 0.f), movementVector.Y);
	AddMovementInput(FVector(0.f, 1.f, 0.f), movementVector.X);
}

void ACharacterBase::ShootAct(FVector pos, FVector direction)
{
	FVector Start = GetActorLocation();
	FVector End = Start + (direction * 10000);
	_ShotDirection = End - Start;

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
	{
		if (ACharacterBase* hitEnemy = Cast<ACharacterBase>(HitResult.GetActor()))
		{
			hitEnemy->DamageCharacter();
		}
	}

	if (APlayerController* playerController = Cast<APlayerController>(Controller))
	{
		DisableInput(playerController);
	}
	_ShotStationaryTimer = ShotStationaryTime;
}



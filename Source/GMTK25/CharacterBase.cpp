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
	FireRateTimer = 0.f;
	_Health = _MaxHealth;
}

void ACharacterBase::DamageCharacter(int damage)
{
	_Health -= damage;
	if (_Health <= 0)
	{
		if(IsPlayer)
			OnPlayerDeath();
		IsAlive = false;
		// Destroy();
	}
}

void ACharacterBase::OnPlayerDeath()
{
	ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->ReloadLevel(true);
	}
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FireRateTimer > 0.f)
	{
		FireRateTimer -= DeltaTime;
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
	if (movementVector.Y > 0 && movementVector.Y > 0)
	{
		OnMoveEvent();
	}
	else
	{
		OnStandStillEvent();
	}
}


#include "CharacterBase.h"
#include "EnemyCharacter.h"
#include "DefaultGameMode.h"
#include "DefaultGameInstance.h"
#include "Kismet/GameplayStatics.h"

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	FireRateTimer = 0.f;
	_Health = _MaxHealth;
}

void ACharacterBase::DamageCharacter(int damage)
{
	_Health -= damage;
	if (_Health <= 0)
	{
		IsAlive = false;
		DeathEvent();
		if(DeathSound && IsValid(DeathSound))
			UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
		SetActorEnableCollision(false);
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


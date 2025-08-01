// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "PlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (_TargetPlayer == nullptr)
    {
        UpdateVision();
    }
    else
    {
        UpdateAggresion(DeltaTime);
    }
}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyCharacter::UpdateVision()
{
	FVector characterPosition = GetActorLocation();
	FVector playerPosition;

	// Get all actors of the player class (assuming you have a player class)
	TArray<AActor*> targets;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacterBase::StaticClass(), targets);

    for (AActor* target : targets)
    {
        if(AEnemyCharacter* fellowEnemy = Cast<AEnemyCharacter>(target))
            continue;

        playerPosition = target->GetActorLocation();
        float distanceToPlayer = FVector::Dist(characterPosition, playerPosition);

        if (distanceToPlayer <= _VisionRange)
        {
            FVector directionToPlayer = (playerPosition - characterPosition).GetSafeNormal();
            FVector characterForward = GetActorForwardVector();

            float angleToPlayer = FMath::Acos(FVector::DotProduct(characterForward, directionToPlayer)) * (180.0f / PI);

            if (angleToPlayer <= _VisionFOW / 2.0f)
            {
                FHitResult hitResult;
                FCollisionQueryParams collisionParams;
                collisionParams.AddIgnoredActor(this);

                if (GetWorld()->LineTraceSingleByChannel(hitResult, characterPosition, playerPosition, ECC_Visibility, collisionParams))
                {
                    if (hitResult.GetActor() == target)
                    {
                        _TargetPlayer = Cast<ACharacterBase>(hitResult.GetActor());
                        _AlertTimer = _TimeToAlert;
                    }
                }
            }
        }
    }
}

void AEnemyCharacter::UpdateAggresion(float DeltaTime)
{
    if (_ShotStationaryTimer > 0.f)
        return;

    FVector characterPosition = GetActorLocation();
	FVector directionToPlayer = _TargetPlayer->GetActorLocation() - characterPosition;
	directionToPlayer.Z = 0;
	directionToPlayer.Normalize();

	if (!directionToPlayer.IsZero())
	{
		FRotator newRotation = directionToPlayer.Rotation();
		SetActorRotation(newRotation);
	}

	if (_AlertTimer > 0.f)
	{
		_AlertTimer -= DeltaTime;

		if (_AlertTimer <= 0.f)
		{
			ShootAct(GetActorLocation(), GetActorForwardVector());
		}
		else
		{
			return;
		}
	}
}
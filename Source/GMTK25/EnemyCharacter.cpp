// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "PlayerCharacter.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DefaultGameMode.h"
#include "Engine/World.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!IsAlive)
        return;

    ADefaultGameMode* GameMode = Cast<ADefaultGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode && GameMode->IsLevelOver())
    {
        return;
    }

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
                        ACharacterBase* targetPlayer = Cast<ACharacterBase>(hitResult.GetActor());
                        if (!targetPlayer->IsAlive)
                            continue;
                        _TargetPlayer = targetPlayer;
                        _AlertTimer = _TimeToAlert;
                        _LoseTargetTimer = _TimeToLoseTarget;
                        OnAlertEvent();
                        IsAlerted = true;
                    }
                    else
                    {
                        IsAlerted = false;
                    }
                }
            }
        }
    }
}

void AEnemyCharacter::UpdateAggresion(float DeltaTime)
{
    if (!IsValid(_TargetPlayer) || !_TargetPlayer->IsAlive)
    {
        _TargetPlayer = nullptr;
        return;
    }
    FHitResult hitResult;
    FCollisionQueryParams collisionParams;
    collisionParams.AddIgnoredActor(this);

    if (_TargetPlayer && GetWorld()->LineTraceSingleByChannel(hitResult, GetActorLocation(), _TargetPlayer->GetActorLocation(), ECC_Visibility, collisionParams))
    {
        if (hitResult.GetActor() != _TargetPlayer)
        {
            _LoseTargetTimer -= DeltaTime;
            if(_LoseTargetTimer <= 0.f)
				_TargetPlayer = nullptr;
            return;
        }
        else
        {
            _LoseTargetTimer = _TimeToLoseTarget;
        }
    }

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
	}
    if (_AlertTimer <= 0.f)
    {
        _AlertTimer = 0.f;
        //UE_LOG(LogTemp, Warning, TEXT("Enemy shot"));
        OnShootEvent();
    }
}
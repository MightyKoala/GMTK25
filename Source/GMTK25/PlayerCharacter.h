// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS( )
class GMTK25_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TakeDamage(int damage = 1);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* _InputContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ShootAction;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	float ShotStationaryTime = 1.f;
	float _ShotStationaryTimer;
	FVector _ShotDirection;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	float _MaxHealth;
	float _Health;

	void Move(const FInputActionValue& value);
	void Shoot(const FInputActionValue& value);
};

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
	APlayerCharacter(bool usePlayerInput);

	struct PlayerFrameRecording
	{
		FVector Location;
		double TimeStamp;
		FVector ForwardVector;
		bool ShootInput;
		FVector ShotPosition;
		float DeltaTime;
		FVector ShotDirection;
		FVector2D MovementInput;
	};

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SimulateFrame(const PlayerFrameRecording& frame);

	void TakeDamage(int damage = 1);

	bool UsePlayerInput = true;

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
	void MoveAct(FVector2D inputVector);
	void Shoot();
	void ShootAct(FVector pos, FVector direction);
};

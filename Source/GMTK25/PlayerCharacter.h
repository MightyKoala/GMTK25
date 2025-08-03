#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "InputActionValue.h"
#include "PlayerFrameRecording.h"
#include "PlayerCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class UCameraComponent;
class USpringArmComponent;

UCLASS( )
class GMTK25_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Shoot")
	void OnShootEvent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* _InputContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ShootAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* FFAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EscapeMenuAction;

	UPROPERTY(EditAnywhere, Category = Input)
	float FastForwardMultiplier = 3.f;
	UPROPERTY(EditAnywhere, Category = Input)
	float TimeDilationSpeed = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* CameraSpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* PlayerCamera;

	void Move(const FInputActionValue& value);
	void Shoot();
	void FastForwardTime();
	void StopFastForward();
	void UpdateTimeDilation(float DeltaTime);
	void TogglePauseMenu();
private:
	PlayerFrameRecording _CurrentFrame;
	float TargetTimeDilation = 1.f;
	float FastforwardLerpValue = 1.f;
};

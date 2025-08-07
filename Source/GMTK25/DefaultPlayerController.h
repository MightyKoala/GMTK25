#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "DefaultPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class GMTK25_API ADefaultPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ADefaultPlayerController();
	virtual void BeginPlay();
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
protected:
	void Move(const FInputActionValue& value);
	void Shoot();
	void TogglePauseMenu();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* InputContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* ShootAction;
	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EscapeMenuAction;
};

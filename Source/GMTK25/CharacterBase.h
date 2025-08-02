#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Sound/SoundCue.h"
#include "CharacterBase.generated.h"

UCLASS(Abstract)
class GMTK25_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Death")
	void DamageCharacter(int damage = 1);

protected:
	virtual void BeginPlay() override;
	void MoveAct(FVector2D inputVector);
	void OnPlayerDeath();
	bool IsPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float FireRate = 0.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Gameplay")
	float FireRateTimer = 0.f;

	UFUNCTION(BlueprintImplementableEvent, Category = "OnMove")
	void OnMoveEvent();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "OnStandStill")
	void OnStandStillEvent();

	UPROPERTY(VisibleAnywhere, Category = "Gameplay")
	int _Health;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	int _MaxHealth = 1;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	USoundCue* GunSound;
};

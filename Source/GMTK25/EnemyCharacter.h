#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class GMTK25_API AEnemyCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Shoot")
	void OnShootEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "Alert")
	void OnAlertEvent();

	UFUNCTION(BlueprintImplementableEvent, Category = "Death")
	void OnDeathEvent();

	UPROPERTY(EditAnywhere, Category = "Sound")
	USoundCue* AlertSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation/EnemyState")
	bool IsAlerted = false;
protected:
	virtual void BeginPlay() override;

	void UpdateVision();
	void UpdateAggresion(float DeltaTime);

	ACharacterBase* _TargetPlayer;

	UPROPERTY(EditAnywhere, Category = "AI")
	float _VisionRange = 10000.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float _VisionFOW = 180.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float _TimeToAlert = 0.9f;
	float _AlertTimer;

	UPROPERTY(EditAnywhere, Category = "AI")
	float _TimeToLoseTarget = 0.9f;
	float _LoseTargetTimer;
};

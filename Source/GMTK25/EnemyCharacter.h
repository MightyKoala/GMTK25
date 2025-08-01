#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class APlayerCharacter;

UCLASS()
class GMTK25_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void TakeDamage(int damage = 1);

protected:
	virtual void BeginPlay() override;

	void UpdateVision();
	void UpdateAggresion(float DeltaTime);

	APlayerCharacter* _TargetPlayer;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	int _Health;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	int _MaxHealth = 1;

	UPROPERTY(EditAnywhere, Category = "AI")
	float _VisionRange = 10000.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float _VisionFOW = 180.f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float _TimeToAlert = 0.9f;
	float _AlertTimer;
};

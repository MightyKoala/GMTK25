#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "DefaultGameMode.generated.h"

UCLASS()
class GMTK25_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADefaultGameMode();

	void RecordFrame(APlayerCharacter::PlayerFrameRecording frame);

protected:

	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, Category = Config)
	float LevelTime = 60.f;
	float LevelTimer;

	UPROPERTY(VisibleAnywhere, Category = Config)
	int AmountOfLives = 3;

	TArray<APlayerCharacter::PlayerFrameRecording> RecordedPlayerFrames;
	int PlayBackIndex;
	
	bool restarted = false;
	float playBackTimer = 0.f;
};

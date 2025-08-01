#pragma once

#include "CoreMinimal.h"
#include "PlayerGhostCharacter.h"
#include "PlayerCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerFrameRecording.h"
#include "DefaultGameMode.generated.h"

UCLASS()
class GMTK25_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADefaultGameMode();

	float GetLevelMaxTime() { return LevelTime; }
	float GetLevelTimer() { return LevelTimer; }
	void ReloadLevel();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SpawnPlayer();
	void SpawnPlayerReplayCharacter(FVector SpawnLocation, FRotator SpawnRotation);
	FVector GetNextSpawnPoint();

	UPROPERTY(EditAnywhere, Category = Config)
	float LevelTime = 60.f;
	float LevelTimer;

	UPROPERTY(EditAnywhere, Category = Config)
	int AmountOfLives = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<APlayerGhostCharacter> PlayerReplayPawn;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<APlayerCharacter> PlayerToSpawn;

	APlayerGhostCharacter* GhostPlayer;
	int PlayBackIndex;
	
	float playBackTimer = 0.f;
};

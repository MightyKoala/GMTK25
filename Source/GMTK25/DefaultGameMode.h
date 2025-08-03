#pragma once

#include "CoreMinimal.h"
#include "PlayerGhostCharacter.h"
#include "PlayerCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerFrameRecording.h"
#include "Blueprint/UserWidget.h"
#include "DefaultGameMode.generated.h"

UCLASS()
class GMTK25_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADefaultGameMode();

	float GetLevelMaxTime() { return LevelTime; }
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	FString GetLevelTimerString() { return FString::Printf(TEXT("%.1f"), LevelTimer); };
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	float GetLevelTimer() { return LevelTimer; }
	void ReloadLevel();
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void UnpauseGame();
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	int GetLivesLeft();
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void CompleteLevel(FString nextLevel);

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	bool IsLevelOver();
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void SetLevelTime(float time);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	float TimeWarpTime = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	float PlayerDeathExtraTime = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	float LevelCompleteTime = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	float LevelStartTime = 1.5;

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void SetLivesLeft(int lives);

	UPROPERTY(BlueprintReadWrite, Category = Config)
	UAudioComponent* TimeRewindComponent;

	UPROPERTY(BlueprintReadWrite, Category = Config)
	UAudioComponent* MusicActorComponent;

	UPROPERTY(BlueprintReadWrite, Category = Config)
	bool IsPlayingTimeWarp = false;

	UPROPERTY(EditAnywhere, Category = Config)
	float LevelTime = 30.f;

	UPROPERTY(BlueprintReadWrite, Category = Config)
	float LevelTimer;
	bool PlayerDied;
	UPROPERTY(BlueprintReadWrite, Category = Config)
	bool LevelCompleted;
	float LevelStartTimer;
	void TogglePauseScreenVisibility();
protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SpawnPlayer();
	void SpawnPlayerReplayCharacter(FVector SpawnLocation, FRotator SpawnRotation);
	FVector GetNextSpawnPoint();
	void ToggleGameOverVisibility();

	float TimeWarpTimer;
	float PlayerDeathTimer;
	float LevelCompleteTimer;
	FString NextLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	int AmountOfLives = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<APlayerGhostCharacter> PlayerReplayPawn;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<APlayerCharacter> PlayerToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	USoundCue* MissionStartSound;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GameOverScreen;
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> PauseScreen;

	UUserWidget* OverlayWidget;

	TArray<APlayerGhostCharacter*> GhostPlayers;
	TArray<int> PlayBackIndexes;

	float PlayBackTimer = 0.f;
};

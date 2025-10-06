#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "FrameRecording.h"
#include "Blueprint/UserWidget.h"
#include "DefaultGameMode.generated.h"

class APlayerGhostCharacter;

UCLASS()
class GMTK25_API ADefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ADefaultGameMode();

	void ReloadLevel();

	UPROPERTY(BlueprintReadWrite, Category = Config)
	UAudioComponent* MusicActorComponent;

	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void TogglePauseScreenVisibility();
	UFUNCTION(BlueprintCallable, Category = "GameFlow")
	void ToggleGameOverVisibility();
	float LevelTimer = 0.f;

	float RecordTime = 5.f;
	float RecordTimer = 0.f;

	void SpawnPlayerReplayCharacter(FVector SpawnLocation, FRotator SpawnRotation);
	void StartRecordingFrames();
protected:
	void StopRecordingFrames();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void SpawnPlayer();
	FVector GetNextSpawnPoint();

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

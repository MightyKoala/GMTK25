#include "DefaultGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSpawnPoint.h"
#include "DefaultGameInstance.h"
#include "Components/AudioComponent.h"

ADefaultGameMode::ADefaultGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	LevelTimer = LevelTime;
	PlayerDied = false;
	LevelCompleted = false;
	NextLevelName = "";
}

void ADefaultGameMode::ReloadLevel()
{
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		GameInstance->IncreaseDeathCount();
		GameInstance->StoreRecordedFrames();

		if (GameInstance->GetDeathCount() >= AmountOfLives)
		{
			ToggleGameOverVisibility();
			return;
		}
	}

	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
		UGameplayStatics::OpenLevel(World, FName(CurrentLevelName));
	}
}

void ADefaultGameMode::UnpauseGame()
{
	TogglePauseScreenVisibility();
}

int ADefaultGameMode::GetLivesLeft()
{
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		return AmountOfLives - GameInstance->GetDeathCount();
	}
	return 0;
}

void ADefaultGameMode::CompleteLevel(FString nextLevel)
{
	LevelCompleted = true;
	NextLevelName = nextLevel;
}

bool ADefaultGameMode::IsLevelOver()
{
	return LevelCompleted || LevelTimer <= 0.f;
}

void ADefaultGameMode::SetLevelTime(float time)
{
	LevelTime = time;
	LevelTimer = LevelTime;
}

void ADefaultGameMode::SetLivesLeft(int lives)
{
	AmountOfLives = lives;
}

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelTimer = LevelTime;
	TimeWarpTimer = TimeWarpTime;
	PlayerDeathTimer = PlayerDeathExtraTime;
	LevelCompleteTimer = LevelCompleteTime;
	LevelStartTimer = LevelStartTime;
	PlayBackTimer = 0.f;
	GhostPlayers.Empty();
	PlayBackIndexes.Empty();

	int deathCount = 0;
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		deathCount = GameInstance->GetDeathCount();
	}

	for (int i = 0; i < deathCount; i++)
	{
		SpawnPlayerReplayCharacter(GetNextSpawnPoint(), FVector::ForwardVector.Rotation());
	}

	SpawnPlayer();
}

void ADefaultGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (LevelStartTimer > 0.f)
	{
		LevelStartTimer -= DeltaTime;

		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController)
		{
			APawn* ControlledPawn = PlayerController->GetPawn();
			if (ControlledPawn)
			{
				// Cast to your specific character class if needed
				APlayerCharacter* player = Cast<APlayerCharacter>(ControlledPawn);
				if (player)
				{
					if (LevelStartTimer <= 0.f)
					{
						player->EnableInput(PlayerController);
					}
					else
					{
						player->DisableInput(PlayerController);
					}
				}
			}
		}
		return;
	}

	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (LevelCompleted)
	{
		LevelCompleteTimer -= DeltaTime;
		if (LevelCompleteTimer <= 0.f)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				if (GameInstance)
				{
					GameInstance->ResetGameInstance();
				}
				if (NextLevelName == "")
				{
					UGameplayStatics::OpenLevel(World, FName("BP_MainMenu"));
				}
				else
				{
					UGameplayStatics::OpenLevel(World, FName(NextLevelName));
				}
			}
		}
		return;
	}

	LevelTimer -= DeltaTime;
	if (LevelTimer <= 0.f)
	{
		LevelTimer = 0.f;
		if (PlayerDied)
		{
			PlayerDeathTimer -= DeltaTime;
			if (PlayerDeathTimer > 0.f)
			{
				return;
			}
		}

		TimeWarpTimer -= DeltaTime;

		if (TimeWarpTimer > 0.f)
		{
			if (IsPlayingTimeWarp == false)
			{
				if(MusicActorComponent)
					MusicActorComponent->SetPitchMultiplier(0.3f);

				if (TimeRewindComponent)
					TimeRewindComponent->Play();

				IsPlayingTimeWarp = true;
			}
		}
		else
		{
			ReloadLevel();
		}
		return;
	}

	if (!GhostPlayers.IsEmpty() && GameInstance)
	{
		//UE_LOG(LogTemp, Warning, TEXT("PlaybackTimer: %f"), PlayBackTimer);
		PlayBackTimer += DeltaTime;

		for (int ghostIndex = 0; ghostIndex < GhostPlayers.Num(); ghostIndex++)
		{
			if (!IsValid(GhostPlayers[ghostIndex]) || !GhostPlayers[ghostIndex]->IsAlive)
				continue;
			int frameCount = GameInstance->GetRecordedPlayerFrames(ghostIndex).Num();
			int lastPlaybackIndex = PlayBackIndexes[ghostIndex];
			for (int frameIndex = lastPlaybackIndex + 1; frameIndex < frameCount; frameIndex++)
			{
				const PlayerFrameRecording& frame = GameInstance->GetRecordedPlayerFrames(ghostIndex)[frameIndex];
				if (frame.TimeStamp < PlayBackTimer)
				{
					if (!IsValid(GhostPlayers[ghostIndex]))
						break;
					//UE_LOG(LogTemp, Warning, TEXT("Replaying frame %d on ghost! from timestamp: %f"), frameIndex, frame.TimeStamp);
					GhostPlayers[ghostIndex]->SimulateFrame(frame);
					PlayBackIndexes[ghostIndex] = frameIndex;
				}
				else
				{
					break;
				}
			}
		}
	}
}

void ADefaultGameMode::SpawnPlayer()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APlayerCharacter* newPlayer = World->SpawnActor<APlayerCharacter>(PlayerToSpawn, GetNextSpawnPoint(), FVector::ForwardVector.Rotation(), SpawnParams);
}

void ADefaultGameMode::SpawnPlayerReplayCharacter(FVector SpawnLocation, FRotator SpawnRotation)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	APlayerGhostCharacter* ghostPlayer = World->SpawnActor<APlayerGhostCharacter>(PlayerReplayPawn, SpawnLocation, SpawnRotation, SpawnParams);
	GhostPlayers.Add(ghostPlayer);
	PlayBackIndexes.Add(0);
}

FVector ADefaultGameMode::GetNextSpawnPoint()
{
	TArray<AActor*> spawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerSpawnPoint::StaticClass(), spawnPoints);
	FVector spawnPos = FVector::Zero();
	int lowestSpawnOrderIndex = -1;
	int lowestSpawnOrder = INT_MAX;

	for (int i = 0; i < spawnPoints.Num(); i++)
	{
		if (APlayerSpawnPoint* currentSpawnPoint = Cast<APlayerSpawnPoint>(spawnPoints[i]))
		{
			int spawnOrder = currentSpawnPoint->GetSpawnOrderIndex();
			if (spawnOrder < lowestSpawnOrder && !currentSpawnPoint->IsAlreadyUsed())
			{
				lowestSpawnOrderIndex = i;
				lowestSpawnOrder = spawnOrder;
			}
		}
	}

	if (lowestSpawnOrderIndex >= 0)
	{
		APlayerSpawnPoint* bestSpawnPoint = Cast<APlayerSpawnPoint>(spawnPoints[lowestSpawnOrderIndex]);
		spawnPos = bestSpawnPoint->GetActorLocation();
		bestSpawnPoint->SetIsUsed(true);
	}
	return spawnPos;
}

void ADefaultGameMode::ToggleGameOverVisibility()
{
	if (!OverlayWidget)
	{
		OverlayWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverScreen);
		if (OverlayWidget)
		{
			OverlayWidget->AddToViewport();
		}
	}

	if (OverlayWidget)
	{
		const ESlateVisibility CurrentVisibility = OverlayWidget->GetVisibility();
		bool isVisible = CurrentVisibility == ESlateVisibility::Visible;

		OverlayWidget->SetVisibility(isVisible ? ESlateVisibility::Hidden : ESlateVisibility::Visible);

		if (isVisible)
		{
			UGameplayStatics::SetGamePaused(GetWorld(), false);
		}
		else
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
	}
}

void ADefaultGameMode::TogglePauseScreenVisibility()
{
	if (!OverlayWidget)
	{
		OverlayWidget = CreateWidget<UUserWidget>(GetWorld(), PauseScreen);
		if (OverlayWidget)
		{
			OverlayWidget->AddToViewport();
		}
	}

	//Reset mouse position to same position as before pausing?
	if (OverlayWidget)
	{
		const ESlateVisibility CurrentVisibility = OverlayWidget->GetVisibility();
		bool isVisible = CurrentVisibility == ESlateVisibility::Visible;

		OverlayWidget->SetVisibility(isVisible ? ESlateVisibility::Hidden : ESlateVisibility::Visible);

		if (isVisible)
		{
			UGameplayStatics::SetGamePaused(GetWorld(), false);
		}
		else
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
	}
}

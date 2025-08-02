#include "DefaultGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSpawnPoint.h"
#include "DefaultGameInstance.h"
#include "Components/AudioComponent.h"

ADefaultGameMode::ADefaultGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	LevelTimer = LevelTime;
}

void ADefaultGameMode::ReloadLevel(bool isCausedByDeath)
{
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		if (isCausedByDeath)
		{
			GameInstance->IncreaseDeathCount();
		}
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

void ADefaultGameMode::CompleteMission()
{
	ToggleCompletionVisibility();
}

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelTimer = LevelTime;
	TimeWarpTimer = TimeWarpTime;
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

	LevelTimer -= DeltaTime;
	if (LevelTimer <= 0.f)
	{
		TimeWarpTimer -= DeltaTime;

		if (TimeWarpTimer > 0.f)
		{
			if (IsPlayingTimeWarp == false)
			{
				MusicActorComponent->SetPitchMultiplier(0.3f);
				TimeRewindComponent->Play();

				IsPlayingTimeWarp = true;
			}
		}
		else
		{
			ReloadLevel(true);
		}
	}

	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GhostPlayers.IsEmpty() && GameInstance)
	{
		PlayBackTimer += DeltaTime;

		for (int ghostIndex = 0; ghostIndex < GhostPlayers.Num(); ghostIndex++)
		{
			if (!IsValid(GhostPlayers[ghostIndex]))
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
					//UE_LOG(LogTemp, Warning, TEXT("Replaying frame %d on ghost!"), frameIndex);
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
	int lowestSpawnOrderIndex = INT_MAX;

	for (AActor* spawnPoint : spawnPoints)
	{
		if (APlayerSpawnPoint* currentSpawnPoint = Cast<APlayerSpawnPoint>(spawnPoint))
		{
			int spawnOrder = currentSpawnPoint->GetSpawnOrderIndex();
			if (spawnOrder < lowestSpawnOrderIndex && !currentSpawnPoint->IsAlreadyUsed())
			{
				lowestSpawnOrderIndex = spawnOrder;
				spawnPos = currentSpawnPoint->GetActorLocation();
				currentSpawnPoint->SetIsUsed(true);
			}
		}
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

//Use on mission objective completion, unify function with other toggle ui function
void ADefaultGameMode::ToggleCompletionVisibility()
{
	if (!OverlayWidget)
	{
		OverlayWidget = CreateWidget<UUserWidget>(GetWorld(), MissionSuccessScreen);
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

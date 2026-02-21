#include "DefaultGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSpawnPoint.h"
#include "DefaultGameInstance.h"
#include "Components/AudioComponent.h"
#include "DefaultPlayerController.h"
#include "PlayerGhostCharacter.h"
#include "GameFramework/PlayerState.h"

ADefaultGameMode::ADefaultGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADefaultGameMode::ReloadLevel()
{
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		GameInstance->IncreaseDeathCount();
		GameInstance->StoreRecordedFrames();
	}

	UWorld* World = GetWorld();
	if (World)
	{
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
		UGameplayStatics::OpenLevel(World, FName(CurrentLevelName));
	}
}

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelTimer = 0.f;
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

	LevelTimer += DeltaTime;

	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (!GhostPlayers.IsEmpty() && GameInstance)
	{
		for (int ghostIndex = 0; ghostIndex < GhostPlayers.Num(); ghostIndex++)
		{
			if (!IsValid(GhostPlayers[ghostIndex]) || !GhostPlayers[ghostIndex]->IsAlive)
				continue;
			int frameCount = GameInstance->GetRecordedFrames(ghostIndex).Num();
			int lastPlaybackIndex = PlayBackIndexes[ghostIndex];
			for (int frameIndex = lastPlaybackIndex + 1; frameIndex < frameCount; frameIndex++)
			{
				const FrameSnapShot& frame = GameInstance->GetRecordedFrames(ghostIndex)[frameIndex];
				if (frame.TimeStamp < LevelTimer)
				{
					if (!IsValid(GhostPlayers[ghostIndex]))
						break;
					GhostPlayers[ghostIndex]->SimulateFrame(frame.PlayerFrame);
					PlayBackIndexes[ghostIndex] = frameIndex;
				}
				else
				{
					break;
				}
			}
		}
	}

	if (GameInstance && RecordTimer > 0.f)
	{
		RecordTimer -= DeltaTime;
		GameInstance->RecordFrame();
		
		if (RecordTimer <= 0.f)
		{
			StopRecordingFrames();
		}
	}
}

void ADefaultGameMode::StartRecordingFrames()
{
	RecordTimer = RecordTime;
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->StoreStartingFrame();
}

void ADefaultGameMode::StopRecordingFrames()
{
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	GameInstance->ResetToStartingFrame();
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
	else if (spawnPoints.Num() > 0) //Backup to prevent spawning bug where player spawns at Vector::Zero for no reason
	{
		APlayerSpawnPoint* backupSpawnPoint = Cast<APlayerSpawnPoint>(spawnPoints[0]);
		spawnPos = backupSpawnPoint->GetActorLocation();
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
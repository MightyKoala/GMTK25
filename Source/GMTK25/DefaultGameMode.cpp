// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSpawnPoint.h"
#include "DefaultGameInstance.h"

ADefaultGameMode::ADefaultGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	LevelTimer = LevelTime;
	GhostPlayer = nullptr;
}

void ADefaultGameMode::ReloadLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("Gamemode reloading level!"));
	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::OpenLevel(World, FName(World->GetMapName()));
	}
}

void ADefaultGameMode::BeginPlay()
{
	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay!"));
	Super::BeginPlay();
	LevelTimer = LevelTime;
	playBackTimer = 0.f;
	GhostPlayer = nullptr;

	int deathCount = 0;
	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GameInstance)
	{
		deathCount = GameInstance->GetDeathCount();
	}

	//Make more able to spawn
	//for (int i = 0; i < deathCount; i++)
	if(deathCount >= 1)
	{
		SpawnPlayerReplayCharacter(GetNextSpawnPoint(), FVector::ForwardVector.Rotation());
		UE_LOG(LogTemp, Warning, TEXT("Spawning replay character!"));
	}

	SpawnPlayer();
}

void ADefaultGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("My level timer is: %f"), LevelTimer);
	LevelTimer -= DeltaTime;
	if (LevelTimer <= 0.f)
	{
		//ReloadLevel();
	}

	UDefaultGameInstance* GameInstance = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GhostPlayer && GameInstance)
	{
		playBackTimer += DeltaTime;
		int frameCount = GameInstance->GetRecordedPlayerFrames().Num();
		if (PlayBackIndex != frameCount - 1)
		{

			for (int i = PlayBackIndex; i < frameCount; i++)
			{
				PlayerFrameRecording& frame = GameInstance->GetRecordedPlayerFrames()[i];
				if (frame.TimeStamp < playBackTimer)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Replaying frame %d on ghost!"), i);
					GhostPlayer->SimulateFrame(frame);
					PlayBackIndex = i;
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

	if (newPlayer)
	{
		// Additional initialization logic can go here
		UE_LOG(LogTemp, Warning, TEXT("Spawned player character!"));
	}
}

void ADefaultGameMode::SpawnPlayerReplayCharacter(FVector SpawnLocation, FRotator SpawnRotation)
{
	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	GhostPlayer = World->SpawnActor<APlayerGhostCharacter>(PlayerReplayPawn, SpawnLocation, SpawnRotation, SpawnParams);

	if (GhostPlayer)
	{
		// Additional initialization logic can go here
		UE_LOG(LogTemp, Warning, TEXT("Spawned replay character!"));
	}
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

// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelManager.h"


// Sets default values
ALevelManager::ALevelManager()
{
	//Level Manager is abandoned
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	/*perlinWidth = 100;
	perlinHeight = 100;
	isLoaded = false;*/
}

// Called when the game starts or when spawned
void ALevelManager::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> foundForestControllers;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AForestController::StaticClass(), foundForestControllers);
	foundForestController = Cast<AForestController>(foundForestControllers[0]);

	SpawnTriggers();

	/*TArray<AActor*> foundForestControllers;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AForestController::StaticClass(), foundForestControllers);
	AForestController* foundForestController = Cast<AForestController>(foundForestControllers[0]);

	TArray<AActor*> foundTerrains;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), APerlinSpawner::StaticClass(), foundTerrains);

	FString screenSizeXStr = foundTerrains[0]->GetClass()->GetName();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);

	for (int i = 0; i < foundTerrains.Num(); i++)
	{
		if (foundTerrains[i]->GetFName().ToString() == PerlinSpawnerName)
		{

			APerlinSpawner* foundTerrain = Cast<APerlinSpawner>(foundTerrains[i]);
			perlinActorLocation = foundTerrain->GetActorLocation();
			UHierarchicalInstancedStaticMeshComponent* foundHISMC = Cast<UHierarchicalInstancedStaticMeshComponent>(foundTerrain->GetComponentByClass(UHierarchicalInstancedStaticMeshComponent::StaticClass()));

			//if (!isLoaded) Doesn't work
			if (!foundForestController->X0Y0Loaded ||PerlinSpawnerName != "PerlinSpawnerX0Y0_2")
			{
				GenerateTerrain(foundForestController, foundHISMC);
				foundForestController->X0Y0 = foundTerrain;
			}
		}
	}*/
}

// Called every frame
void ALevelManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelManager::GenerateTerrain(AForestController* ForestController, UHierarchicalInstancedStaticMeshComponent* hismc)
{
	for (int y = perlinActorLocation.Y/100 - perlinHeight / 2; y < perlinActorLocation.Y/100 + perlinHeight / 2; ++y)
	{
		for (int x = perlinActorLocation.X/100 - perlinWidth / 2; x < perlinActorLocation.X/100 + perlinWidth / 2; ++x)
		{
			ForestController->InstantiateHISMC(x, y, hismc, perlinActorLocation);
		}
	}
	hismc->SetWorldLocation(FVector(0, 0, 0));
	isLoaded = true;
}

void ALevelManager::SpawnTriggers()
{
	//Top Right
	for (int y = 0; y < foundForestController->mapSize * 100; y += 5000)
	{
		for (int x = 0; x < foundForestController->mapSize * 100;x += 5000)
		{
			FActorSpawnParameters spawnParams;
			//AActor* numActor = GetWorld()->SpawnActor<ALevelStreamingTrigger>(ALevelStreamingTrigger::StaticClass(), FVector(0,0,0), FRotator::ZeroRotator, spawnParams);
			AActor* numActor = GetWorld()->SpawnActor<ALevelStreamingTrigger>(ALevelStreamingTrigger::StaticClass(), FVector(x, y, 0), FRotator::ZeroRotator, spawnParams);
		}
	}
	// Bottom Left
	for (int y = - foundForestController->mapSize * 100; y < 0; y += 5000)
	{
		for (int x = - foundForestController->mapSize * 100; x < 0; x += 5000)
		{
			FActorSpawnParameters spawnParams;
			//AActor* numActor = GetWorld()->SpawnActor<ALevelStreamingTrigger>(ALevelStreamingTrigger::StaticClass(), FVector(0,0,0), FRotator::ZeroRotator, spawnParams);
			AActor* numActor = GetWorld()->SpawnActor<ALevelStreamingTrigger>(ALevelStreamingTrigger::StaticClass(), FVector(x, y, 0), FRotator::ZeroRotator, spawnParams);
		}
	}
	//Top Left
	for (int y = 0; y < foundForestController->mapSize * 100; y += 5000)
	{
		for (int x = -foundForestController->mapSize * 100; x < 0; x += 5000)
		{
			FActorSpawnParameters spawnParams;
			//AActor* numActor = GetWorld()->SpawnActor<ALevelStreamingTrigger>(ALevelStreamingTrigger::StaticClass(), FVector(0,0,0), FRotator::ZeroRotator, spawnParams);
			AActor* numActor = GetWorld()->SpawnActor<ALevelStreamingTrigger>(ALevelStreamingTrigger::StaticClass(), FVector(x, y, 0), FRotator::ZeroRotator, spawnParams);
		}
	}
	//Bottom Right
	for (int y = -foundForestController->mapSize * 100; y < 0; y += 5000)
	{
		for (int x = 0; x < foundForestController->mapSize * 100; x += 5000)
		{
			FActorSpawnParameters spawnParams;
			//AActor* numActor = GetWorld()->SpawnActor<ALevelStreamingTrigger>(ALevelStreamingTrigger::StaticClass(), FVector(0,0,0), FRotator::ZeroRotator, spawnParams);
			AActor* numActor = GetWorld()->SpawnActor<ALevelStreamingTrigger>(ALevelStreamingTrigger::StaticClass(), FVector(x, y, 0), FRotator::ZeroRotator, spawnParams);
		}
	}
}



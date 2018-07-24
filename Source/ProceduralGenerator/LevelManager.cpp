// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelManager.h"


// Sets default values
ALevelManager::ALevelManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	perlinWidth = 200;
	perlinHeight = 200;
	isLoaded = false;
}

// Called when the game starts or when spawned
void ALevelManager::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> foundForestControllers;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AForestController::StaticClass(), foundForestControllers);
	AForestController* foundForestController = Cast<AForestController>(foundForestControllers[0]);

	TArray<AActor*> foundTerrains;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), APerlinSpawner::StaticClass(), foundTerrains);

	for (int i = 0; i < foundTerrains.Num(); i++)
	{
		//FString screenSizeXStr = foundTerrains[i]->GetFName().ToString();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
		if (foundTerrains[i]->GetFName().ToString() == PerlinSpawnerName)
		{
			FString screenSizeXStr = "Works!";
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);

			APerlinSpawner* foundTerrain = Cast<APerlinSpawner>(foundTerrains[i]);
			perlinActorLocation = foundTerrain->GetActorLocation();
			UHierarchicalInstancedStaticMeshComponent* foundHISMC = Cast<UHierarchicalInstancedStaticMeshComponent>(foundTerrain->GetComponentByClass(UHierarchicalInstancedStaticMeshComponent::StaticClass()));

			if (!isLoaded)
			{
				GenerateTerrain(foundForestController, foundHISMC);
			}
		}
	}
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


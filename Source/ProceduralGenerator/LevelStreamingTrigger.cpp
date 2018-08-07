// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelStreamingTrigger.h"

// LevelStreamingTrigger Spawns Perlin in the beginning, then show/hide them later
// This strat seems to fail tremendously.  What if I make it such that it moves the perlin noise to another location, and modify the hism instead, such that the hism follows where the player goes

// Sets default values
ALevelStreamingTrigger::ALevelStreamingTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	OverlapVolume->bGenerateOverlapEvents = true;
	RootComponent = OverlapVolume;
	isOverlappedAtLeastOnce = false;

	perlinWidth = 50;
	perlinHeight = 50;
	isLoaded = false;

	XBound = 15000;
	YBound = 15000;
	ZBound = 60000;

	OverlapVolume->SetBoxExtent(FVector(XBound, YBound, ZBound));
	//OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelStreamingTrigger::OverlapBegins);
	//OverlapVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &ALevelStreamingTrigger::OverlapEnds);

}

// Called when the game starts or when spawned
void ALevelStreamingTrigger::BeginPlay()
{
	Super::BeginPlay();
	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelStreamingTrigger::OverlapBegins);
	OverlapVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &ALevelStreamingTrigger::OverlapEnds);

}

// Called every frame
void ALevelStreamingTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelStreamingTrigger::GenerateTerrain(AForestController* ForestController, UHierarchicalInstancedStaticMeshComponent* hismc)
{
	for (int y = perlinActorLocation.Y / 100 - perlinHeight / 2; y < perlinActorLocation.Y / 100 + perlinHeight / 2; ++y)
	{
		for (int x = perlinActorLocation.X / 100 - perlinWidth / 2; x < perlinActorLocation.X / 100 + perlinWidth / 2; ++x)
		{
			ForestController->InstantiateHISMC(x, y, hismc, perlinActorLocation);
		}
	}
	hismc->SetWorldLocation(FVector(0, 0, 0));
	isLoaded = true;
}


void ALevelStreamingTrigger::GenerateHISMCInstances()
{
	TArray<AActor*> foundForestControllers;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), AForestController::StaticClass(), foundForestControllers);
	AForestController* foundForestController = Cast<AForestController>(foundForestControllers[0]);

	//TArray<AActor*> foundTerrains;
	//UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), APerlinSpawner::StaticClass(), foundTerrains);

	FActorSpawnParameters spawnParams;
	//AActor* perlinActor = GetWorld()->SpawnActor<APerlinSpawner>(FVector(0, 0, 0), FRotator::ZeroRotator, spawnParams);
	AActor* perlinActor = GetWorld()->SpawnActor<APerlinSpawner>(this->GetActorLocation(), FRotator::ZeroRotator, spawnParams);

	//for (int i = 0; i < foundTerrains.Num(); i++)
	//{
		//if (foundTerrains[i]->GetFName().ToString() == PerlinSpawnerName)
		//{

			//APerlinSpawner* foundTerrain = Cast<APerlinSpawner>(foundTerrains[i]);
			PerlinSpawner = Cast<APerlinSpawner>(perlinActor);
			//perlinActorLocation = foundTerrain->GetActorLocation();
			perlinActorLocation = PerlinSpawner->GetActorLocation();
			//UHierarchicalInstancedStaticMeshComponent* foundHISMC = Cast<UHierarchicalInstancedStaticMeshComponent>(foundTerrain->GetComponentByClass(UHierarchicalInstancedStaticMeshComponent::StaticClass
			UHierarchicalInstancedStaticMeshComponent* foundHISMC = Cast<UHierarchicalInstancedStaticMeshComponent>(PerlinSpawner->GetComponentByClass(UHierarchicalInstancedStaticMeshComponent::StaticClass()));

			//if (!isLoaded) Doesn't work
			//if (!foundForestController->X0Y0Loaded || PerlinSpawnerName != "PerlinSpawnerX0Y0_2")
			//if (!foundForestController->X0Y0Loaded)
			//{
				GenerateTerrain(foundForestController, foundHISMC);
			//}
		//}
	//}
}

void ALevelStreamingTrigger::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (!isOverlappedAtLeastOnce)
		{

			/* Good news: The triggers can differentiate themselves
			FString screenSizeXStr = this->GetName();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);*/

			//if (OtherActor == MyCharacter && LevelToLoad != "")
			if (OtherActor == MyCharacter)
			{
				//FLatentActionInfo LatentInfo;
				//UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
				GenerateHISMCInstances();
				isOverlappedAtLeastOnce = true;
			}
		}
		else
		{
			PerlinSpawner->SetActorHiddenInGame(false);
		}
	}
	
}
void ALevelStreamingTrigger::OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//UGameplayStatics::GetAllActorsWithTag(this->GetWorld(), "PerlinSpawnerX0Y0", foundTerrains);
	//PerlinSpawner = Cast<APerlinSpawner>(foundTerrains[0]);

	//FString screenSizeXStr = "Leaver1!!!";
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
     	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		//FString screenSizeXStr = "Leaver2!!!";
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
		ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		//if (OtherActor == MyCharacter && LevelToLoad != "")
		if (OtherActor == MyCharacter)
		{
			//FString screenSizeXStr = "Leaver!!!";
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
			/*FLatentActionInfo LatentInfo;
			UGameplayStatics::UnloadStreamLevel(this, LevelToLoad, LatentInfo);*/
			PerlinSpawner->SetActorHiddenInGame(true);
		}
	}
}



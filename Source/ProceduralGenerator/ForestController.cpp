// Fill out your copyright notice in the Description page of Project Settings.

#include "ForestController.h"
#include "FastNoise.h"


//General Log
DEFINE_LOG_CATEGORY(LogMyGame);
// Sets default values
AForestController::AForestController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialize land spawn values
	perlinWidth = 50;
	perlinHeight = 50;
	perlinSeed = FMath::RandRange(0, 2000);
	perlinMaxHeight = 50;
	noiseDampX = 0.3;
	noiseDampY = 0.3;
	objectSpawnZOffset = { 0, 0, 525 };




}

// Called when the game starts or when spawned
void AForestController::BeginPlay()
{
	Super::BeginPlay();

	//Perlin Noise:
	PerlinNoise pn(perlinSeed);

	noiseOffsetY = 0;

	if (GetWorld()) {
		int axisArrayYCounter = 0;
		for (int y = 0; y < perlinHeight; ++y)
		{
			noiseOffsetY += noiseDampY;
			noiseOffsetX = 0;

			arrayOfTerrainGenerationAxises.AddUninitialized(1, 0);

			int axisArrayXCounter = 0;
			for (int x = 0; x < perlinWidth; ++x)
			{
				arrayOfTerrainGenerationAxises.Columns[axisArrayYCounter].AddNewRow();
				noiseOffsetX += noiseDampX;
				//double perlinX = (double)x / (double)perlinWidth - 0.5;
				double perlinX = (double) noiseOffsetX / (double)perlinWidth - 0.5;
				//double perlinY = (double)y / (double)perlinHeight - 0.5;
				double perlinY = (double) noiseOffsetY / (double)perlinHeight - 0.5;

				randomHeight = (double)FMath::RandRange(0, 10) * (double)0.1;
				FVector SpawnLoc = FVector(x, y, pn.noise(10 * perlinX, 10 * perlinY, 0.8) * perlinMaxHeight - (perlinMaxHeight / 4.5));

//FString TheFloatStr = FString::SanitizeFloat((float)pn.noise(10 * perlinX, 10 * perlinY, 0.8));
//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *TheFloatStr);
FRotator SpawnRot = FRotator(0, 0, 0);

FActorSpawnParameters perlinSpawnParams;
FVector vectorToOrigin = { SpawnLoc.X * 50, SpawnLoc.Y * 50, 0 };
FVector theOffset = { -2000, -2000, 0 };
GetWorld()->SpawnActor<APerlinSpawner>(whatToSpawn, SpawnLoc * 100 + theOffset, SpawnRot, perlinSpawnParams);
arrayOfTerrainGenerationAxises.Columns[axisArrayYCounter].Rows[axisArrayXCounter] = SpawnLoc * 100 + theOffset;

axisArrayXCounter++;
			}

			axisArrayYCounter++;
		}
	}

	generateOcean();



	for (int i = 0; i < arrayOfTerrainGenerationAxises.Columns.Num(); i++) {
		for (int j = 0; j < arrayOfTerrainGenerationAxises.Columns[i].Rows.Num(); j++) {
			ArrayOfGeneratedX.Add(arrayOfTerrainGenerationAxises.Columns[i].Rows[j].X);
			ArrayOfGeneratedY.Add(arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Y);
			ArrayOfGeneratedZ.Add(arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Z);
		}
	}

	//Generate assets
	FVector newVector = { 0,0,0 };
	for (int i = 0; i < 100; i++)
	{
		//newX = generateX(newX);
		//newY = generateY(newY);

		newVector = generateVector();
		

		//oldVectors[i] = newX;
		oldVectors.Add(newVector);

		const UWorld* world = GetWorld();

		if (world)
		{
			//create spawn params
			FActorSpawnParameters spawnParams;
			spawnParams.Owner = this;
			spawnParams.Instigator = Instigator;

			//create a vector
			//FVector newVec;
			//newVec.Set(newX, newY, 150.0f);

			//pick an object
			int num = FMath::RandRange(0, 1);

			if (num == 0)
			{
				AActor* numActor = GetWorld()->SpawnActor<AProceduralTree>(objectOne, newVector + objectSpawnZOffset, FRotator::ZeroRotator, spawnParams);

			}
			else if (num == 1)
			{
				AActor* numActor = GetWorld()->SpawnActor<AProceduralTree>(objectTwo, newVector + objectSpawnZOffset, FRotator::ZeroRotator, spawnParams);
			}
		}



	}

}

// Called every frame
void AForestController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UE_LOG(LogMyGame, Warning, TEXT("print"));
}

FVector AForestController::generateVector()
{
	float indexY = FMath::FRandRange(0, arrayOfTerrainGenerationAxises.Columns.Num() - 1);
	float indexX = FMath::FRandRange(0, arrayOfTerrainGenerationAxises.Columns[0].Rows.Num() - 1);

	FVector passBack = arrayOfTerrainGenerationAxises.Columns[indexY].Rows[indexX];

	/*if (isOccupied(oldVectors, passBack))
	{
		//generate new X if overused
		passBack = generateVector();
	}*/

	return passBack;
}

bool AForestController::isOccupied(TArray<FVector>& oldArray, FVector vectorCheck)
{
	bool used = false;
	for (int i = 0; i < oldArray.Num(); i++)
	{
		if (isDistanced(oldArray[i], vectorCheck))
		{
			used = true;
		}
	}
	return used;
}

bool AForestController::isDistanced(FVector inOne, FVector inTwo)
{
	bool passBack = false;
	if (FGenericPlatformMath::Abs(inOne.X - inTwo.X) < 50 
		&& FGenericPlatformMath::Abs(inOne.Y - inTwo.Y) < 50
		&& FGenericPlatformMath::Abs(inOne.Y - inTwo.Y) < 50) 
	{
		passBack = true;
	}
	return passBack;
}

void AForestController::generateOcean()
{

	//Calculate position for Ocean relative to the generated terrain
	maxX = -10000;
	minX = 10000;
	maxY = -10000;
	minY = 10000;

	noOfGrids = 0;

	for (int i = 0; i < arrayOfTerrainGenerationAxises.Columns.Num(); i++)
	{
		for (int j = 0; j < arrayOfTerrainGenerationAxises.Columns[i].Rows.Num(); j++)
		{
			if (arrayOfTerrainGenerationAxises.Columns[i].Rows[j].X < minX) {
				minX = arrayOfTerrainGenerationAxises.Columns[i].Rows[j].X;
			}
			if (arrayOfTerrainGenerationAxises.Columns[i].Rows[j].X > maxX) {
				maxX = arrayOfTerrainGenerationAxises.Columns[i].Rows[j].X;
			}
			if (arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Y < minY) {
				minY = arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Y;
			}
			if (arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Y > maxY) {
				maxY = arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Y;
			}
			if (arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Z < minZ) {
				minZ = arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Z;
			}
			if (arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Z > maxZ) {
				maxZ = arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Z;
			}

			zValues.Add(arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Z);
			noOfGrids++;
		}
	}

	zValues.Sort([](const double& LHS, const double& RHS) {return LHS < RHS; });
	float zVal = zValues[FMath::FloorToFloat(landHeightPercentileCoveredByWater * (float)noOfGrids)-1]; //0 doesn't work; fix later
	FVector OceanSpawnLoc = { (float) (minX + maxX) / 2, (float) (minY + maxY) / 2, zVal };
	FRotator OceanSpawnRot = FRotator(0, 0, 0);
	FActorSpawnParameters oceanSpawnParams;
	GetWorld()->SpawnActor<AActor>(oceanBluePrint, OceanSpawnLoc, OceanSpawnRot,oceanSpawnParams);

	//postProcessOceanVolume->Set;
	FActorSpawnParameters oceanVolumeSpawnParams;
	FVector deductionVector = { 0, 0, OceanSpawnLoc.Z / 2 };
	AOceanVolume* OceanVolumeActor = GetWorld()->SpawnActor<AOceanVolume>(postProcessOceanVolume, OceanSpawnLoc - deductionVector, OceanSpawnRot, oceanVolumeSpawnParams);
	oceanVolumelengthX = maxX - minX;
	oceanVolumeWidthY = maxY - minY;
	oceanVolumeHeightZ = OceanSpawnLoc.Z / 2 - minZ-75;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *TheFloatStr);

	FVector BoxSize = { oceanVolumelengthX,oceanVolumeWidthY,oceanVolumeHeightZ };
	OceanVolumeActor->SetBoxSize(BoxSize);


}



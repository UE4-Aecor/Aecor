// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralTree.h"
#include "GameFramework/Actor.h"
#include "PerlinNoise.h"
#include "PerlinSpawner.h"
#include "Engine/PostProcessVolume.h"
#include "OceanVolume.h"
#include "ForestController.generated.h"

//General Log
DECLARE_LOG_CATEGORY_EXTERN(LogMyGame, Log, All);

USTRUCT()
struct FTerrainGenerationColumn
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		TArray<FVector> Rows;

	void AddNewRow()
	{
		FVector newVec = { 0, 0, 0 };
		Rows.Add(newVec);
	}
	//default properties
	FTerrainGenerationColumn()
	{

	}
};

USTRUCT()
struct FTerrainGenerationGrid
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		TArray<FTerrainGenerationColumn> Columns;

	void AddNewColumn()
	{
		Columns.Add(FTerrainGenerationColumn());
	}

	void AddUninitialized(const int ColCount, const int RowCount)
	{
		//Add Columns
		for (int v = 0; v < ColCount; v++)
		{
			AddNewColumn();
		}

		//Add Rows
		for (int v = 0; v < ColCount; v++)
		{
			for (int b = 0; b < RowCount; b++)
			{
				Columns[v].AddNewRow();
			}
		}
	}

	void Clear()
	{
		/*if (Columns.Num() <= 0) return;
		//~~~~~~~~~~~~~~~

		//Destroy any walls
		const int32 ColTotal = Columns.Num();
		const int32 RowTotal = Columns[0].Rows.Num();

		for (int32 v = 0; v < ColTotal; v++)
		{
			for (int32 b = 0; b < RowTotal; b++)
			{
				//if (Columns[v].Rows[b] && Columns[v].Rows[b]->IsValidLowLevel())
				//{
				Columns[v].Rows[b]->Destroy();
				//}
			}
		}

		//Empty
		for (int32 v = 0; v < Columns.Num(); v++)
		{
			Columns[v].Rows.Empty();
		}
		Columns.Empty();*/
	}

	//default properties
	FTerrainGenerationGrid()
	{

	}
};

UCLASS()
class PROCEDURALGENERATOR_API AForestController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AForestController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	FVector generateVector();

	bool isOccupied(TArray<FVector>& oldArray, FVector vectorCheck); //check if the value is been used before using addr

	bool isDistanced(FVector inOne, FVector inTwo); //check dist between pts

	void generateOcean();

private:
	TArray<FVector> oldVectors; //Array to hold all the old Xs

	int perlinSeed;
	double randomHeight;
	double noiseOffsetX = 0;
	double noiseOffsetY = 0;

	TArray<double> ArrayOfGeneratedX;
	TArray<double> ArrayOfGeneratedY;
	TArray<double> ArrayOfGeneratedZ;

	double maxX;
	double minX;
	double maxY;
	double minY;
	double maxZ;
	double minZ;

	double noOfGrids;
	TArray<double> zValues;

public:
	float oceanVolumelengthX;
	float oceanVolumeWidthY;
	float oceanVolumeHeightZ;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spawning Object")
	TSubclassOf<AProceduralTree> objectOne;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning Object")
	TSubclassOf<AProceduralTree> objectTwo;

	UPROPERTY(EditAnywhere, Category = "Spawning Object")
	FVector objectSpawnZOffset;

	UPROPERTY(EditAnywhere, Category = "Spawning Land")
	int perlinWidth;
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
	int perlinHeight;
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
	TSubclassOf<class APerlinSpawner> whatToSpawn;

	UPROPERTY(EditAnywhere, Category = "Spawning Land")
	double perlinMaxHeight;
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
	double noiseDampX;
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
	double noiseDampY;

	UPROPERTY(EditAnywhere, Category = "Spawning Ocean")
	TSubclassOf<class AActor> oceanBluePrint;
	//Percentile of all heights in the array covered by water (between 0 to 1)
	UPROPERTY(EditAnywhere, Category = "Spawning Ocean")
	double landHeightPercentileCoveredByWater; //NOTE: Have to manually adjust the BP_OceanWater scale

	UPROPERTY(EditAnywhere, Category = "Spawning Ocean")
	TSubclassOf<class AOceanVolume> postProcessOceanVolume;

	//Now you have dynamic array benefits and also UPROPERTY()!
	UPROPERTY()
	FTerrainGenerationGrid arrayOfTerrainGenerationAxises;
	

	
};
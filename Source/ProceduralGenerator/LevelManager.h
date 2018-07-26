// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForestController.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "LevelManager.generated.h"

UCLASS()
class PROCEDURALGENERATOR_API ALevelManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	void GenerateTerrain(AForestController* foundForestController,UHierarchicalInstancedStaticMeshComponent* hismc);

protected:
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
	int perlinWidth;
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
	int perlinHeight;
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
	FString PerlinSpawnerName;

private:
	FVector perlinActorLocation;
	bool isLoaded;
};

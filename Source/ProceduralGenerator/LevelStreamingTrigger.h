// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "PerlinSpawner.h"
#include "ForestController.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "LevelStreamingTrigger.generated.h"

UCLASS()
class PROCEDURALGENERATOR_API ALevelStreamingTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelStreamingTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void GenerateTerrain(AForestController* ForestController, UHierarchicalInstancedStaticMeshComponent* hismc);
	void GenerateHISMCInstances();

protected:
	UPROPERTY(EditAnywhere)
		FName LevelToLoad;

private:
	// Overlap volume to trigger level streaming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UBoxComponent* OverlapVolume;

	UFUNCTION()
		void OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
		void OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//void OverlapEnds(AActor* OverlappedActor, AActor* OtherActor);


private:
	TArray<AActor*> foundTerrains;
	APerlinSpawner* PerlinSpawner;
	bool isOverlappedAtLeastOnce;

protected:
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
		int perlinWidth;
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
		int perlinHeight;
	UPROPERTY(EditAnywhere, Category = "Spawning Land")
		FString PerlinSpawnerName;
	UPROPERTY(EditAnywhere, Category = "Trigger Bounds")
		int XBound;
	UPROPERTY(EditAnywhere, Category = "Trigger Bounds")
		int YBound;
	UPROPERTY(EditAnywhere, Category = "Trigger Bounds")
		int ZBound;

private:
	FVector perlinActorLocation;
	bool isLoaded;

};

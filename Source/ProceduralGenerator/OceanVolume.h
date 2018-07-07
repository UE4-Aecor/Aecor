// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PhysicsVolume.h"
#include "Components/PostProcessComponent.h"
#include "Components/BoxComponent.h"
#include "OceanVolume.generated.h"

UCLASS()
class PROCEDURALGENERATOR_API AOceanVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOceanVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetBoxSize(FVector boxDimension);

//To set the size of postprocesscomponent
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
UBoxComponent* BoxContainer;

/** Pawn mesh: 1st person view  */
UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
UPostProcessComponent* PostProcessComp;


};

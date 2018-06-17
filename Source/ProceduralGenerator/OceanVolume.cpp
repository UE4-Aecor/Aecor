// Fill out your copyright notice in the Description page of Project Settings.

#include "OceanVolume.h"

// Sets default values
AOceanVolume::AOceanVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxContainer = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxContainer"));
	RootComponent = BoxContainer;

	PostProcessComp = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessing"));
	PostProcessComp->SetupAttachment(BoxContainer);
}

// Called when the game starts or when spawned
void AOceanVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOceanVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOceanVolume::SetBoxSize(FVector boxDimension)
{
	BoxContainer->SetBoxExtent(boxDimension);
}



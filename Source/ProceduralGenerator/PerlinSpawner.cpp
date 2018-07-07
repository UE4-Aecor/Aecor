// Fill out your copyright notice in the Description page of Project Settings.

#include "PerlinSpawner.h"


// Sets default values
APerlinSpawner::APerlinSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	hismc = CreateDefaultSubobject<UHierarchicalInstancedStaticMeshComponent>(TEXT("HInstancedStaticMeshComp"));
	RootComponent = hismc;
}

void APerlinSpawner::OnConstruction(const FTransform & Transform)
{

	RegisterAllComponents();
	hismc->ClearInstances();

	UStaticMesh* Rockmesh = LoadObject<UStaticMesh>(nullptr, TEXT("StaticMesh'/Game/StarterContent/Props/SM_Rock.SM_Rock'"));
	hismc->SetStaticMesh(Rockmesh);

	hismc->GetBodySetup()->CollisionTraceFlag = ECollisionTraceFlag::CTF_UseComplexAsSimple;
	hismc->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;
	hismc->CanCharacterStepUp(UGameplayStatics::GetPlayerPawn(this->GetWorld(), 0));
	hismc->bGenerateOverlapEvents = true;
	hismc->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	hismc->SetCollisionResponseToAllChannels(ECR_Block);
	hismc->SetMobility(EComponentMobility::Static);

	AForestController* ForestController = GetWorld()->SpawnActor<AForestController>();
	ForestController->generateTerrain(hismc);
	ForestController->SetActorEnableCollision(true);

}

// Called when the game starts or when spawned
void APerlinSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APerlinSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


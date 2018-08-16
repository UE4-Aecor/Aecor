// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryRobot.h"


// Sets default values
AInventoryRobot::AInventoryRobot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	bUseVelocityChange = false;
	MovementForce = 1500;
	RequiredDistanceToTarget = 200;
}

// Called when the game starts or when spawned
void AInventoryRobot::BeginPlay()
{
	Super::BeginPlay();

	//Find Initial Moveto
	NextPathPoint = GetNextPathPoint();
}

// Called every frame
void AInventoryRobot::Tick(float DeltaTime)
{
	//Perhaps, I should update the pathpoint more frequently?
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		//Keep moving towards next target
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();

		ForceDirection *= MovementForce;

		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() * ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
	}

	//DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 1.0f);

}

FVector AInventoryRobot::GetNextPathPoint()
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, this->GetActorLocation(), Player);
	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1]; 
	}
	return GetActorLocation();
}

// Called to bind functionality to input
/*void AInventoryRobot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
*/

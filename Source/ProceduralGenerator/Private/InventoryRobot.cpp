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

	//testing
	//static ConstructorHelpers::FObjectFinder<UBehaviorTree> InventoryRobotBTObj(TEXT("BehaviorTree'/Game/Characters/InventoryRobotBT.InventoryRobotBT'"));
	//InventoryRobotBT = InventoryRobotBTObj.Object;

}

// Called when the game starts or when spawned
void AInventoryRobot::BeginPlay()
{
	Super::BeginPlay();

	//Find Initial Moveto
	//NextPathPoint = GetNextPathPoint();

	InventoryRobotAIController = UAIBlueprintHelperLibrary::GetAIController(this);
	//AController* ThisPawnController = this->GetController();
	//InventoryRobotAIController = Cast<AAIController>(ThisPawnController);
	
	InventoryRobotBT = LoadObject<UBehaviorTree>(nullptr, TEXT("BehaviorTree'/Game/Characters/InventoryRobotBT.InventoryRobotBT'"));
	if (InventoryRobotBT)
	{

		InventoryRobotAIController->RunBehaviorTree(InventoryRobotBT);
	}

	//InventoryRobotBB = LoadObject<UBlackboardComponent>(nullptr, TEXT("BlackboardData'/Game/Characters/InventoryRobotBB.InventoryRobotBB'"));
	InventoryRobotBB = UAIBlueprintHelperLibrary::GetBlackboard(this);
	if (InventoryRobotBB)
	{
		InventoryRobotBB->SetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation"), FVector(10000,10000,488));
		//InventoryRobotBB->SetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation"), UGameplayStatics::GetPlayerCharacter(this->GetWorld(), 0)->GetActorLocation());
		FString screenSizeXStr = "XStr:" + FString::SanitizeFloat((float)InventoryRobotBB->GetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation")).X);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
		screenSizeXStr = "YStr:" + FString::SanitizeFloat((float)InventoryRobotBB->GetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation")).Y);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
		screenSizeXStr = "ZStr:" + FString::SanitizeFloat((float)InventoryRobotBB->GetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation")).Z);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
	}
}

// Called every frame
void AInventoryRobot::Tick(float DeltaTime)
{
	//Perhaps, I should update the pathpoint more frequently?
	Super::Tick(DeltaTime);

	//For Testing Purpose
	if (InventoryRobotBB)
	{
		//InventoryRobotBB->SetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation"), UGameplayStatics::GetPlayerCharacter(this->GetWorld(), 0)->GetActorLocation());
		//InventoryRobotBB->SetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation"), FVector(0,0,0));

		/*FString screenSizeXStr = "XStr:" + FString::SanitizeFloat((float)InventoryRobotBB->GetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation")).X);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
		screenSizeXStr = "YStr:" + FString::SanitizeFloat((float)InventoryRobotBB->GetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation")).Y);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
		screenSizeXStr = "ZStr:" + FString::SanitizeFloat((float)InventoryRobotBB->GetValueAsVector(UKismetSystemLibrary::MakeLiteralName("FlyToLocation")).Z);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);*/

		this->AddMovementInput(FVector(1, 0, 0), 1,true);
	}


	//Uncomment this later, if the test doesn't work
	/*
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
	}*/

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
	/*
	if (GetDonPathPoint()->PathPoints.Num() > 1)
	{
		return GetDonPathPoint()->PathPoints[1];
	}
	*/
	return GetActorLocation();
	
}

// Called to bind functionality to input
/*void AInventoryRobot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
*/

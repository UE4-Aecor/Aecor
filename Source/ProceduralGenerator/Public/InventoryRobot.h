// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BehaviorTree.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "InventoryRobot.generated.h"

UCLASS()
class PROCEDURALGENERATOR_API AInventoryRobot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AInventoryRobot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly,Category = "Components")
	UStaticMeshComponent* MeshComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget;

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "TrackerBot")
		UNavigationPath*  GetDonPathPoint();

	UFUNCTION(BlueprintImplementableEvent, Category = "TrackerBot")
		UNavigationPath*  FlyToPathPoint();

	//FVector GetDonPathPoint();

private:

	AAIController* InventoryRobotAIController;
	UBehaviorTree* InventoryRobotBT;
	UBlackboardComponent* InventoryRobotBB;
	
};

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
	perlinWidth = 200;
	perlinHeight = 200;
	perlinSeed = FMath::RandRange(0, 2000);
	perlinMaxHeight = 150;
	noiseDampX = 0.3;
	noiseDampY = 0.3;
	objectSpawnZOffset = { 0, 0, 525 };
	landHeightPercentileCoveredByWater = 0.95;
	minRaycastOffset = 100;
	maxRaycastOffset = 10000;
	raycastApartOffset = { 1,1,1 };
	//objectOne = LoadObject<AProceduralTree>(nullptr, TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree1'"));
	//objectTwo = LoadObject<AProceduralTree>(nullptr, TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree2'"));
	//ConstructorHelpers::FObjectFinder<AProceduralTree> objectOne(TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree1'"));
	//ConstructorHelpers::FObjectFinder<AProceduralTree> objectTwo(TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree2'"));
	SpawnObjectsComplete = false;






}

// Called when the game starts or when spawned
void AForestController::BeginPlay()
{
	Super::BeginPlay();

	//generateTerrain();

	generateOcean();

	for (int i = 0; i < arrayOfTerrainGenerationAxises.Columns.Num(); i++) {
		for (int j = 0; j < arrayOfTerrainGenerationAxises.Columns[i].Rows.Num(); j++) {
			ArrayOfGeneratedX.Add(arrayOfTerrainGenerationAxises.Columns[i].Rows[j].X);
			ArrayOfGeneratedY.Add(arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Y);
			ArrayOfGeneratedZ.Add(arrayOfTerrainGenerationAxises.Columns[i].Rows[j].Z);
		}
	}

	UGameplayStatics::GetAllActorsWithTag(this->GetWorld(), "WaterCaustics", foundWaterCaustics);

	SpawnObjects();

	SpawnPlayer();

}

// Called every frame
void AForestController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!SpawnObjectsComplete)
	{
		objectOne = LoadObject<AProceduralTree>(nullptr, TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree1'"));
		objectTwo = LoadObject<AProceduralTree>(nullptr, TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree2'"));
		if (objectOne && objectTwo && objectOne != nullptr && objectTwo != nullptr)
		{
			SpawnObjects();
		}
	}
	//detectViewportVisibilityByRaycast();

	if (OceanSpawnActor && OceanSpawnActor != nullptr)
	{
		if (foundWaterCaustics[0])
		{
			foundWaterCausticsComponent = Cast<ADirectionalLight>(foundWaterCaustics[0]);
			if (UGameplayStatics::GetPlayerPawn(this->GetWorld(), 0)->GetActorLocation().Z > OceanSpawnLoc.Z - 100)
			{
				foundWaterCausticsComponent->GetLightComponent()->SetMobility(EComponentMobility::Movable);
				foundWaterCausticsComponent->GetLightComponent()->SetVisibility(false);

			}
			else
			{
				foundWaterCausticsComponent->GetLightComponent()->SetMobility(EComponentMobility::Movable);
				foundWaterCausticsComponent->GetLightComponent()->SetVisibility(true);
			}
		}
	}


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
void AForestController::generateTerrain(UHierarchicalInstancedStaticMeshComponent* hismc)
{
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
				double perlinX = (double)noiseOffsetX / (double)perlinWidth - 0.5;
				//double perlinY = (double)y / (double)perlinHeight - 0.5;
				double perlinY = (double)noiseOffsetY / (double)perlinHeight - 0.5;

				randomHeight = (double)FMath::RandRange(0, 10) * (double)0.1;
				FVector SpawnLoc = FVector(x, y, pn.noise(10 * perlinX, 10 * perlinY, 0.8) * perlinMaxHeight - (perlinMaxHeight / 4.5));
				FRotator SpawnRot = FRotator(0, 0, 0);

				FVector vectorToOrigin = { SpawnLoc.X * 50, SpawnLoc.Y * 50, 0 };
				FVector theOffset = { -2000, -2000, 0 };
				//For testing purposes:
				hismc->AddInstance(FTransform(SpawnLoc * 100 + theOffset));

				arrayOfTerrainGenerationAxises.Columns[axisArrayYCounter].Rows[axisArrayXCounter] = SpawnLoc * 100 + theOffset;

				axisArrayXCounter++;
			}

			axisArrayYCounter++;
		}
	}
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
	OceanSpawnLoc = { (float) (minX + maxX) / 2, (float) (minY + maxY) / 2, zVal };
	OceanSpawnRot = FRotator(0, 0, 0);
	FActorSpawnParameters oceanSpawnParams;
	//GetWorld()->SpawnActor<AActor>(oceanBluePrint->GetClass(), OceanSpawnLoc, OceanSpawnRot,oceanSpawnParams);
	TArray<AActor*> foundOceanSpawns;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "OceanSpawn", foundOceanSpawns);
	OceanSpawnActor = Cast<AActor>(foundOceanSpawns[0]);
	OceanSpawnActor->SetActorLocationAndRotation(OceanSpawnLoc, OceanSpawnRot);

	//postProcessOceanVolume->Set;
	FActorSpawnParameters oceanVolumeSpawnParams;
	FVector deductionVector = { 0, 0, OceanSpawnLoc.Z / 2 };
	//AOceanVolume* OceanVolumeActor = GetWorld()->SpawnActor<AOceanVolume>(postProcessOceanVolume, OceanSpawnLoc - deductionVector, OceanSpawnRot, oceanVolumeSpawnParams);
	//Test:
	TArray<AActor*> foundOceanVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOceanVolume::StaticClass(), foundOceanVolumes);
	AOceanVolume* OceanVolumeActor = Cast<AOceanVolume>(foundOceanVolumes[0]);
	OceanVolumeActor->SetActorLocationAndRotation(OceanSpawnLoc - deductionVector, OceanSpawnRot);

	oceanVolumelengthX = maxX - minX;
	oceanVolumeWidthY = maxY - minY;
	oceanVolumeHeightZ = OceanSpawnLoc.Z / 2 - minZ-75;
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *TheFloatStr);

	FVector BoxSize = { oceanVolumelengthX,oceanVolumeWidthY,oceanVolumeHeightZ };
	OceanVolumeActor->SetBoxSize(BoxSize);

	TSubclassOf<APhysicsVolume> classToFind;
	classToFind = APhysicsVolume::StaticClass();
	TArray<AActor*> foundPhysicsVolume;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), classToFind, foundPhysicsVolume);

	foundPhysicsVolume[0]->SetActorScale3D(BoxSize);
	foundPhysicsVolume[0]->SetActorLocation(OceanSpawnLoc - deductionVector);
}

void AForestController::SpawnObjects()
{
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
				AActor* numActor = GetWorld()->SpawnActor<AProceduralTree>(AProceduralTree::StaticClass(), newVector + objectSpawnZOffset, FRotator::ZeroRotator, spawnParams);

			}
			else if (num == 1)
			{
				AActor* numActor = GetWorld()->SpawnActor<AProceduralTree>(AProceduralTree::StaticClass(), newVector + objectSpawnZOffset, FRotator::ZeroRotator, spawnParams);
			}
		}



	}
	SpawnObjectsComplete = true;
}

//I can see how loading meshes can take a lot of time, hopefully this can help minimizing the runtime
void AForestController::detectViewportVisibilityByRaycast()
{
	/*TArray<AActor*> CurrentlyRenderedActors;
	TArray<AActor*> CurrentlyNotRenderedActors;
	//Visibility__GetRenderedActors(UGameplayStatics::GetPlayerCharacter(GetWorld(),0), onScreenActors, 0.01f);
	//Visibility__GetNotRenderedActors(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), notOnScreenActors, 0.01f);


	UWorld* const World = GEngine->GetWorldFromContextObject(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0), EGetWorldErrorMode::ReturnNull);
	if (!World) return;
	//~~~~~~~~~~~

	//Empty any previous entries
	CurrentlyRenderedActors.Empty();
	CurrentlyNotRenderedActors.Empty();

	//Iterate Over Actors
	for (TActorIterator<AActor> Itr(World); Itr; ++Itr)
	{
		if (World->GetTimeSeconds() - Itr->GetLastRenderTime() <= 0.01f)
		{
			CurrentlyRenderedActors.Add(*Itr);
		} 
		else 
		{
			CurrentlyNotRenderedActors.Add(*Itr);
		}
	}

	for (int i = 0; i < CurrentlyRenderedActors.Num(); i++) 
	{
		if (CurrentlyRenderedActors[i]->IsA(APerlinSpawner::StaticClass()))
		{
			APerlinSpawner* spawner = Cast<APerlinSpawner>(CurrentlyRenderedActors[i]);
			spawner->SetActorHiddenInGame(true);
			spawner->SetActorEnableCollision(false);
			//spawner->SetActorTickEnabled(false);
		}
		if (CurrentlyNotRenderedActors[i]->IsA(APerlinSpawner::StaticClass()))
		{
			APerlinSpawner* spawner = Cast<APerlinSpawner>(CurrentlyNotRenderedActors[i]);
			spawner->SetActorHiddenInGame(false);
			spawner->SetActorEnableCollision(true);
			//spawner->SetActorTickEnabled(true);
		}
	}



	/*FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	//Re-initialize hit info
	FHitResult RV_Hit(ForceInit);
	ACharacter* myCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	FVector cameraRot = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraRotation().Vector();

	FVector cameraDir = myCharacter->GetActorLocation() - UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
	cameraDir.Normalize();
	//TArray<UActorComponent*> characterGunList = myCharacter->GetComponentsByTag(UParticleSystemComponent::StaticClass(),"Gun");
	//UActorComponent* characterGun = characterGunList[0];
	//FVector Start = Cast<USceneComponent>(characterGun)->GetComponentLocation(); // Need Setup

	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());


	APlayerController* myPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FVector ScreenWorldLocation;
	FVector ScreenWorldDirection;
	FVector StartWorldLocation;
	FVector EndWorldLocation;
	for (int x = 1; x < ViewportSize.X - 1; x += 100) 
	{
		for (int y = 1; y < ViewportSize.Y - 1; y += 100)
		{
			if (myPlayerController->DeprojectScreenPositionToWorld(x, y, ScreenWorldLocation, ScreenWorldDirection)) //If Screen position exists
			{
				StartWorldLocation = ScreenWorldLocation;
				//Note: It's location, not rotation!<----- . ----->
				EndWorldLocation = StartWorldLocation + cameraRot * 1000 * raycastApartOffset;
				GetWorld()->LineTraceSingleByChannel(
					RV_Hit,        //result
					StartWorldLocation,    //start
					EndWorldLocation, //end
					ECC_WorldStatic, //collision channel
					RV_TraceParams
				);

				DrawDebugLine(
					GetWorld(),
					StartWorldLocation,
					EndWorldLocation,
					FColor(255, 0, 0),
					false, -1, 0,
					1
				);



				//After hit, get these stuff:
				if (RV_Hit.GetActor())
				{
					RV_Hit.bBlockingHit; //did hit something? (bool)
					RV_Hit.GetActor(); //the hit actor if there is one
					RV_Hit.ImpactPoint;  //FVector
					RV_Hit.ImpactNormal;  //FVector
					FString actorNameStr = RV_Hit.GetActor()->GetName();
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *actorNameStr);
					FString screenSizeXStr = FString::SanitizeFloat((float)ViewportSize.X);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
					FString screenSizeYStr = FString::SanitizeFloat((float)ViewportSize.Y);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeYStr);
				}
			}
		}
	}*/
}

void AForestController::SpawnPlayer()
{
	APawn* mainCharacter = UGameplayStatics::GetPlayerPawn(this->GetWorld(), 0);
	mainCharacter->SetActorTransform(FTransform(FVector((minX + maxX) / 2, (minY + maxY) / 2, maxZ + 50)));
}


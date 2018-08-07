// Fill out your copyright notice in the Description page of Project Settings.

#include "ForestController.h"
#include "FastNoise.h"


//General Log
DEFINE_LOG_CATEGORY(PerlinLog);
// Sets default values
AForestController::AForestController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialize land spawn values
	perlinWidth = 600;//200
	perlinHeight = 600;//200
	perlinSeed = FMath::RandRange(0, 2000);
	perlinMaxHeight = 150;//150
	//noiseDampX = 0.3;
	//noiseDampY = 0.3;
	noiseDampX = 0.3f;
	noiseDampY = 0.3f;
	objectSpawnZOffset = { 0, 0, 525 };
	landHeightPercentileCoveredByWater = 0.99;
	minRaycastOffset = 100;
	maxRaycastOffset = 10000;
	raycastApartOffset = { 1,1,1 };
	//objectOne = LoadObject<AProceduralTree>(nullptr, TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree1'"));
	//objectTwo = LoadObject<AProceduralTree>(nullptr, TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree2'"));
	//ConstructorHelpers::FObjectFinder<AProceduralTree> objectOne(TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree1'"));
	//ConstructorHelpers::FObjectFinder<AProceduralTree> objectTwo(TEXT("Blueprint'/Game/ProceduralTree1.ProceduralTree2'"));
	SpawnObjectsComplete = false;
	//total map size is in (mapsize km * mapsize km)
	mapSize = 1200;
	isInitialLevel = false;

	//X0Y0Loaded = false;
	deathTimer = 0;





}

// Called when the game starts or when spawned
void AForestController::BeginPlay()
{
	Super::BeginPlay();

	//generateTerrain();

	FLatentActionInfo LatentInfo;
	UGameplayStatics::UnloadStreamLevel(this, "/Game/Levels/UEDPIE_0_CentralQuadrantLeft", LatentInfo);
	UGameplayStatics::UnloadStreamLevel(this, "/Game/Levels/UEDPIE_0_CentralQuadrantRight", LatentInfo);

	TArray<AActor*> foundTerrains;
	
	UGameplayStatics::GetAllActorsWithTag(this->GetWorld(), "PerlinSpawner", foundTerrains);
	APerlinSpawner* foundTerrain = Cast<APerlinSpawner>(foundTerrains[0]);
	perlinActorLocation = foundTerrain->GetActorLocation();
	UHierarchicalInstancedStaticMeshComponent* foundHISMC = Cast<UHierarchicalInstancedStaticMeshComponent>(foundTerrain->GetComponentByClass(UHierarchicalInstancedStaticMeshComponent::StaticClass()));
	generateTerrain(foundHISMC);

	//FActorSpawnParameters perlinSpawnParams;
	//APerlinSpawner* perlinSpawner = GetWorld()->SpawnActor<APerlinSpawner>(perlinSpawner->GetClass(), FVector(0,0,0), FRotator(0, 0, 0), perlinSpawnParams);

	findMinMaxAndZVals(arrayOfTerrainGenerationAxises);

	generateOcean();

	UGameplayStatics::GetAllActorsWithTag(this->GetWorld(), "WaterCaustics", foundWaterCaustics);
	UGameplayStatics::GetAllActorsWithTag(this->GetWorld(), "OceanFog", foundFogs);

	//SpawnObjects();

	SpawnPlayer();


	if (CharacterBarsWidgetTemplate)
	{
		CharacterBarsWidget = CreateWidget<UMyUserWidget>(this->GetGameInstance(), CharacterBarsWidgetTemplate);
		CharacterBarsWidget->AddToViewport();
	}

	if (UCanvasPanel* OuterPanel = Cast<UCanvasPanel>(CharacterBarsWidget->GetWidgetFromName("OuterPanel")))
	{
		TArray<UWidget*> ArrayOfWidgets;
		CharacterBarsWidget->WidgetTree->GetChildWidgets(OuterPanel, ArrayOfWidgets);

		for (int i = 0; i < ArrayOfWidgets.Num(); i++)
		{

			if (ArrayOfWidgets[i]->GetName() == "BackgroundBlur_0")
			{
				backgroundBlur = Cast<UBackgroundBlur>(ArrayOfWidgets[i]);
				backgroundBlur->SetRenderOpacity(0);

			}
			if (ArrayOfWidgets[i]->GetName() == "BloodOverlay")
			{
				BloodOverlay = ArrayOfWidgets[i];
				BloodOverlay->SetRenderOpacity(0);

			}
			if (ArrayOfWidgets[i]->GetName() == "VignetteOverlay")
			{
				VignetteOverlay = ArrayOfWidgets[i];
				VignetteOverlay->SetRenderOpacity(0);

			}
			if (ArrayOfWidgets[i]->GetName() == "CompleteBlackout")
			{
				CompleteBlackout = ArrayOfWidgets[i];
				CompleteBlackout->SetRenderOpacity(0);

			}
		}

		/*
		FString screenSizeXStr = FString::SanitizeFloat((float)backgroundBlur->GetDesiredSize().Y);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);*/
	}
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

		if (foundFogs[0])
		{
			foundFogActor = Cast<AExponentialHeightFog>(foundFogs[0]);
			if (UGameplayStatics::GetPlayerPawn(this->GetWorld(), 0)->GetActorLocation().Z > OceanSpawnLoc.Z - 100)
			{
				//foundFog Actor->GetComponent()->SetFogDensity(1);
				foundFogActor->SetActorHiddenInGame(true);

			}
			else
			{
				//set this back after testing
				foundFogActor->SetActorHiddenInGame(false);
			}
		}
	}

	/*TArray<AActor*> foundTerrains;
	UGameplayStatics::GetAllActorsOfClass(this->GetWorld(), APerlinSpawner::StaticClass(), foundTerrains);
	for (int i = 0; i < foundTerrains.Num(); i++)
	{
		FString levelName = foundTerrains[i]->GetLevel()->GetOuter()->GetName();
		if (levelName == "SafeShallowsX0Y0")
		{
			X0Y0Loaded = true;
		}
	}*/

	if (UGameplayStatics::GetPlayerCharacter(this->GetWorld(), 0)->GetActorLocation().Z < zVal - 100)
	{
		CharacterBarsWidget->initialAirEfficiency -= DeltaTime/30; //deltatime = 1
	}
	else
	{
		CharacterBarsWidget->initialAirEfficiency = 1;
	}
	if (backgroundBlur && VignetteOverlay && BloodOverlay && CompleteBlackout)
	{
		//Blurry effect for Thirst, Hunger and Air Efficiency
		if (CharacterBarsWidget->initialThirst <= 0.1 || CharacterBarsWidget->initialHunger <= 0.1 || CharacterBarsWidget->initialAirEfficiency <= 0.2)
		{
			float majorDeterminent = FMath::Min3(CharacterBarsWidget->initialThirst, CharacterBarsWidget->initialHunger, CharacterBarsWidget->initialAirEfficiency);

			backgroundBlur->SetRenderOpacity(1 - majorDeterminent * 5);//0.2 * 5 = 1 (Blurriest); 0.0 * 5 = 0 (No Blur);
		}
		else {
			if (backgroundBlur->GetRenderOpacity() > 0)
			{
				backgroundBlur->SetRenderOpacity(backgroundBlur->GetRenderOpacity() - 0.01);
			}
		}
		
		//Special Vignette effect for air efficiency aside from adjusting the blurriness
		if (CharacterBarsWidget->initialAirEfficiency <= 0.2 || CharacterBarsWidget->initialHealth <= 0.2)
		{
			float majorDeterminent = FGenericPlatformMath::Min(CharacterBarsWidget->initialAirEfficiency, CharacterBarsWidget->initialHealth);
			VignetteOverlay->SetRenderOpacity(1 - majorDeterminent * 5);//0.2 * 5 = 1 (Blurriest); 0.0 * 5 = 0 (No Blur);
		}
		else
		{
			if (VignetteOverlay->GetRenderOpacity() > 0)
			{
				VignetteOverlay->SetRenderOpacity(VignetteOverlay->GetRenderOpacity() - 0.01);
			}
		}


		//Blood Effect for Health
		if (CharacterBarsWidget->initialHealth <= 0.2)
		{
			BloodOverlay->SetRenderOpacity((1 - CharacterBarsWidget->initialHealth * 5) / 2);//0.2 * 5 = 1 (Blurriest); 0.0 * 5 = 0 (No Blur);
		} 
		else {
			if (BloodOverlay->GetRenderOpacity() > 0)
			{
				BloodOverlay->SetRenderOpacity(BloodOverlay->GetRenderOpacity() - 0.01);
			}
		}

		if (CharacterBarsWidget->initialHealth <= 0 || CharacterBarsWidget->initialAirEfficiency <= 0)
		{
			deathTimer += DeltaTime;
			CompleteBlackout->SetRenderOpacity(deathTimer / 5);

			if (deathTimer >= 5)
			{
				FString screenSizeXStr = "GAME OVER!";
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
			}
		}
		else 
		{
			if (CompleteBlackout->GetRenderOpacity() > 0)
			{
				deathTimer = 0;
				CompleteBlackout->SetRenderOpacity(CompleteBlackout->GetRenderOpacity() - 0.01);
			}
		}
	}



	CharacterBarsWidget->initialHunger -= DeltaTime /(60 * 48);
	CharacterBarsWidget->initialThirst -= DeltaTime /(60 * 24);

	if (CharacterBarsWidget->initialHunger <= 0 || CharacterBarsWidget->initialThirst <= 0)
	{
		CharacterBarsWidget->initialHealth -= DeltaTime / 5;
	}

}

FVector AForestController::generateVector()
{
	float indexY = 0;
	float indexX = 0;
	int arrayChooser = 0;
	FVector passBack = { 0,0,0 };


	indexY = FMath::FRandRange(0, arrayOfTerrainGenerationAxises.Columns.Num() - 1);
	indexX = FMath::FRandRange(0, arrayOfTerrainGenerationAxises.Columns[0].Rows.Num() - 1);
	passBack = arrayOfTerrainGenerationAxises.Columns[indexY].Rows[indexX];

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


void AForestController::InstantiateHISMC(int x, int y, UHierarchicalInstancedStaticMeshComponent* hismc, FVector perlinLoc)
{
	if (y >= perlinLoc.Y / 100 - perlinHeight / 2
		&& y < perlinLoc.Y / 100 + perlinHeight / 2
		&& x >= perlinLoc.X / 100 - perlinWidth / 2
		&& x < perlinLoc.X / 100 + perlinWidth / 2)
	{//Because spawn loc is 100 times larger than x or y
		//hismc->AddInstance(FTransform(SpawnLoc * 100 + theOffset));
		//if (y < 25 && y > -25 && x < 25 && x > -25)
		if (y == 0 && x == 0)
		{
			//FString screenSizeXStr = FString::SanitizeFloat((float)arrayOfTerrainGenerationAxises.Columns[mapSize / 2].Rows[mapSize / 2].Z);
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
		}
		if (x < 600 && x > -601 && y < 600 && y > -601) 
		{
			if (arrayOfTerrainGenerationAxises.Columns.Num() != 0)
			hismc->AddInstance(FTransform(arrayOfTerrainGenerationAxises.Columns[y + mapSize / 2].Rows[x + mapSize / 2]));
		}
	}
}

//The slope takes around 50m*50m of space
void AForestController::GenerateSlopeHeightmap(int x, int y, int slopeOuterLengthX, int slopeOuterLengthY, bool isUphill, bool isDownhill)
{// 1. X: (250 to 300)||(-250 to -300) & Y: (250 to 300)||(-250 to -300) 
 // 2. X: (249 to -249) & Y: (250 to 300)||(-250 to -300) 
 // 3. X: (250 to 300)||(-250 to -300) & Y: (249 to -249)
 // 4. X: (249 to -249) & Y: (249 to -249)

 // 1. X: (300 to 350)||(-300 to -350) & Y: (300 to 350)||(-300 to -350)
 // 2. X: (299 to -299) & Y: (300 to 350)||(-300 to -350)
 // 3. X: (300 to 350)||(-300 to -350) & Y: (299 to -299)
 // 4. X: (299 to -299) & Y: (299 to -299)
	if (((x <= slopeOuterLengthX / 2 && x >= slopeOuterLengthX / 2 - 50) || (x <= -slopeOuterLengthX / 2 + 50 && x >= -slopeOuterLengthX / 2))
		&& ((y <= slopeOuterLengthY / 2 && y >= slopeOuterLengthY / 2 - 50) || (y <= -slopeOuterLengthY / 2 + 50 && y >= -slopeOuterLengthY / 2)))
	{
		hypotenuseFromOrigin = FGenericPlatformMath::Sqrt(FGenericPlatformMath::Pow(FGenericPlatformMath::Abs(x) - (slopeOuterLengthX / 2 - 50), 2) + FGenericPlatformMath::Pow(FGenericPlatformMath::Abs(y) - (slopeOuterLengthY / 2 - 50), 2));
		maxHypotenuseFromOrigin = FGenericPlatformMath::Sqrt(FGenericPlatformMath::Pow(50, 2) + FGenericPlatformMath::Pow(50, 2));
		scaledFactor = hypotenuseFromOrigin * 1.4 / maxHypotenuseFromOrigin;
	}
	if (x <= slopeOuterLengthX / 2 - 51 && x >= -slopeOuterLengthX / 2 + 51
		&& ((y <= slopeOuterLengthY / 2 && y >= slopeOuterLengthY / 2 - 50) || (y <= -slopeOuterLengthY / 2 + 50 && y >= -slopeOuterLengthY / 2)))
	{
		hypotenuseFromOrigin = FGenericPlatformMath::Sqrt(FGenericPlatformMath::Pow(FGenericPlatformMath::Abs(y) - (slopeOuterLengthY / 2 - 50), 2));
		maxHypotenuseFromOrigin = FGenericPlatformMath::Sqrt(FGenericPlatformMath::Pow(50, 2) + FGenericPlatformMath::Pow(50, 2));
		scaledFactor = hypotenuseFromOrigin * 1.4 / maxHypotenuseFromOrigin;
	}
	if (((x <= slopeOuterLengthX / 2 && x >= slopeOuterLengthX / 2 - 50) || (x <= -slopeOuterLengthX / 2 + 50 && x >= -slopeOuterLengthX / 2))
		&& y <= slopeOuterLengthY / 2 - 51 && y >= -slopeOuterLengthY / 2 + 51)
	{
		hypotenuseFromOrigin = FGenericPlatformMath::Sqrt(FGenericPlatformMath::Pow(FGenericPlatformMath::Abs(x) - (slopeOuterLengthX / 2 - 50), 2));
		maxHypotenuseFromOrigin = FGenericPlatformMath::Sqrt(FGenericPlatformMath::Pow(50, 2) + FGenericPlatformMath::Pow(50, 2));
		scaledFactor = hypotenuseFromOrigin * 1.4 / maxHypotenuseFromOrigin;
	}
	if (x <= slopeOuterLengthX / 2 - 51 && x >= -slopeOuterLengthX / 2 + 51 && y <= slopeOuterLengthY / 2 - 51 && y >= -slopeOuterLengthY / 2 + 51)
	{
		scaledFactor = 0;
	}
	if (isDownhill)
	{
		scaledFactor = 1 - scaledFactor;
	}
}

void AForestController::generateTerrain(UHierarchicalInstancedStaticMeshComponent* hismc)
{
	//Perlin Noise:
	PerlinNoise pn(perlinSeed);

	noiseOffsetY = 0;
	int testcounter = 0;

	if (GetWorld()) {


		//for (int y = 0; y < perlinHeight; ++y)
		//for (int y = perlinActorLocation.Y - perlinHeight/2; y < perlinActorLocation.Y + perlinHeight/2; ++y)
		//Note: Cannot be in the same class, as the smaller class need to call on the array the ForestController Owns
		for (int y = -mapSize / 2; y < mapSize / 2; ++y)
		{//600 - 400/2 = 400; 600 + 400/2 = 800; -600 - 400/2 = -800; -600 + 400/2 = -400 <-- formula correct


				noiseOffsetY += noiseDampY;

				noiseOffsetX = 0;

				arrayOfTerrainGenerationAxises.AddUninitialized(1, 0);

				//for (int x = 0; x < perlinWidth; ++x)
				int counterDeduction = 0;
				for (int x = -mapSize / 2; x < mapSize / 2; ++x)
				{

					//arrayOfTerrainGenerationAxises.Columns[axisArrayYCounter].AddNewRow()
					//Quadrants won't work
					FVector SpawnLoc;
					FRotator SpawnRot;

					//if (x <= perlinWidth/2 && x >= -perlinWidth/2 && y <= perlinHeight/2 && y >= -perlinHeight/2
					//	|| (x <= 800 / 2 && x >= -800 / 2 && y <= 800 / 2 && y >= -800 / 2
					//		&& !(x <= 700 / 2 && x >= -700 / 2 && y <= 700 / 2 && y >= -700 / 2)))
				//	{	
						//Without trying extra damp:
						//hypotenuseFromOrigin = FGenericPlatformMath::Sqrt(FGenericPlatformMath::Pow(x,2)+ FGenericPlatformMath::Pow(y, 2));
						//maxHypotenuseFromOrigin = FGenericPlatformMath::Sqrt(FGenericPlatformMath::Pow(100, 2) + FGenericPlatformMath::Pow(100, 2));
						//float scaledFactor = hypotenuseFromOrigin * 1.27 / maxHypotenuseFromOrigin;

						//With trying extra damp:

						//200,200: pw start=(100,50),(-50,100); maxhypfromorigin bet. (0,50) hfo = x-50
						//300,300: pw start=(150,100),(-100,150); maxhypfromorigin bet. (0,50)  hfo = x-100
						//600,600: pw start=(300,250),(-250,-300); maxhypfromorigin bet. (0,50)  hfo = x-250

						scaledFactor = 1;

						if (x <= perlinWidth / 2 && x >= -perlinWidth / 2 && y <= perlinHeight / 2 && y >= -perlinHeight / 2)
						{
							GenerateSlopeHeightmap(x, y, perlinWidth, perlinHeight, true, false);
						}
						if (x <= 800 / 2 && x >= -800 / 2 && y <= 800 / 2 && y >= -800 / 2
							&& !(x <= 700 / 2 && x >= -700 / 2 && y <= 700 / 2 && y >= -700 / 2))
						{
							GenerateSlopeHeightmap(x, y, 800, 800, false, true);
						}
						if (x <= 1200 / 2 && x >= -1200 / 2 && y <= 1200 / 2 && y >= -1200 / 2
							&& !(x <= 800 / 2 && x >= -800 / 2 && y <= 800 / 2 && y >= -800 / 2))
						{
							scaledFactor = 0;
						}
						
						noiseOffsetX += noiseDampX * (double) scaledFactor;
						noiseOffsetY = noiseOffsetY - noiseDampY + noiseDampY * (double)scaledFactor;

						double perlinX = (double)noiseOffsetX / (double)perlinWidth - 0.5;
						double perlinY = (double)noiseOffsetY / (double)perlinHeight - 0.5;

						float scaledMaxHeight = perlinMaxHeight * scaledFactor;
						SpawnLoc = FVector(x, y, pn.noise(10 * perlinX, 10 * perlinY, 0.8) * scaledMaxHeight - (scaledMaxHeight / 4.5));
						SpawnRot = FRotator(0, 0, 0);
						noiseOffsetY = noiseOffsetY + noiseDampY - noiseDampY * (double) scaledFactor;
				/*	}
					else {

						noiseOffsetX += noiseDampX;

						//double perlinX = (double)x / (double)perlinWidth - 0.5;
						double perlinX = (double)noiseOffsetX / (double)perlinWidth - 0.5;
						//double perlinY = (double)y / (double)perlinHeight - 0.5;
						double perlinY = (double)noiseOffsetY / (double)perlinHeight - 0.5;

						//May have to modify this value for a particular range --> for perlinMaxHeight, and then plug it back into the array
						SpawnLoc = FVector(x, y, pn.noise(10 * perlinX, 10 * perlinY, 0.8) * perlinMaxHeight - (perlinMaxHeight / 4.5));
						SpawnRot = FRotator(0, 0, 0);
					}*/

					FVector vectorToOrigin = { SpawnLoc.X * 50, SpawnLoc.Y * 50, 0 };
					FVector theOffset = { 50, 50, 0 };

					arrayOfTerrainGenerationAxises.Columns[y + mapSize / 2].AddNewRow();
					arrayOfTerrainGenerationAxises.Columns[y + mapSize / 2].Rows[x + mapSize / 2] = SpawnLoc * 100 + theOffset;

					//for testing purpose:
					//RIGHT NOW, ONLY LEVEL MANAGER WOULD GENERATE LANDSCAPE

					//if (FGenericPlatformMath::Fmod(FGenericPlatformMath::Abs(x), 200) == 0 && FGenericPlatformMath::Fmod(FGenericPlatformMath::Abs(y), 200))
					//{

					//}

					/*if (x <= 125 && x >= -125 && y <= 125 && y >= -125)
					{

					}
					else
					{
						InstantiateHISMC(x, y, hismc, perlinActorLocation);
					}*/

					/*
						screenSizeXStr = "noiseDampY is:" + FString::SanitizeFloat((float)noiseDampY);
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
						UE_LOG(PerlinLog, Warning, TEXT("noiseDampY is: %s"), *screenSizeXStr);*/
			}
		}
	}
	FString screenSizeXStr = FString::SanitizeFloat((float)testcounter);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
}
void AForestController::findMinMaxAndZVals(FTerrainGenerationGrid terrainGenArray)
{
	//Calculate position for Ocean relative to the generated terrain
	maxX = -10000;
	minX = 10000;
	maxY = -10000;
	minY = 10000;

	noOfGrids = 0;

	for (int i = 0; i < terrainGenArray.Columns.Num(); i++)
	{
		for (int j = 0; j < terrainGenArray.Columns[i].Rows.Num(); j++)
		{
			if (terrainGenArray.Columns[i].Rows[j].X < minX) {
				minX = terrainGenArray.Columns[i].Rows[j].X;
			}
			if (terrainGenArray.Columns[i].Rows[j].X > maxX) {
				maxX = terrainGenArray.Columns[i].Rows[j].X;
			}
			if (terrainGenArray.Columns[i].Rows[j].Y < minY) {
				minY = terrainGenArray.Columns[i].Rows[j].Y;
			}
			if (terrainGenArray.Columns[i].Rows[j].Y > maxY) {
				maxY = terrainGenArray.Columns[i].Rows[j].Y;
			}
			if (terrainGenArray.Columns[i].Rows[j].Z < minZ) {
				minZ = terrainGenArray.Columns[i].Rows[j].Z;
			}
			if (terrainGenArray.Columns[i].Rows[j].Z > maxZ) {
				maxZ = terrainGenArray.Columns[i].Rows[j].Z;
			}

			zValues.Add(terrainGenArray.Columns[i].Rows[j].Z);
			noOfGrids++;
		}
	}
}

void AForestController::generateOcean()
{

	zValues.Sort([](const double& LHS, const double& RHS) {return LHS < RHS; });

	zVal = zValues[FMath::FloorToFloat(landHeightPercentileCoveredByWater * (float)noOfGrids)-1]; //0 doesn't work; fix later
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
	//oceanVolumeHeightZ = OceanSpawnLoc.Z / 2.95f - minZ-75;
	oceanVolumeHeightZ = OceanSpawnLoc.Z / 2;
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


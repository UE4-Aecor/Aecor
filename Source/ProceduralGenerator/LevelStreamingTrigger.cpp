// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelStreamingTrigger.h"


// Sets default values
ALevelStreamingTrigger::ALevelStreamingTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	OverlapVolume->bGenerateOverlapEvents = true;
	RootComponent = OverlapVolume;

	//OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelStreamingTrigger::OverlapBegins);
	//OverlapVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &ALevelStreamingTrigger::OverlapEnds);
}

// Called when the game starts or when spawned
void ALevelStreamingTrigger::BeginPlay()
{
	Super::BeginPlay();
	OverlapVolume->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALevelStreamingTrigger::OverlapBegins);
	OverlapVolume->OnComponentEndOverlap.AddUniqueDynamic(this, &ALevelStreamingTrigger::OverlapEnds);
}

// Called every frame
void ALevelStreamingTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelStreamingTrigger::OverlapBegins(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (OtherActor == MyCharacter && LevelToLoad != "")
		{
			FString screenSizeXStr = "Triggered!!!";
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
			FLatentActionInfo LatentInfo;
			UGameplayStatics::LoadStreamLevel(this, LevelToLoad, true, true, LatentInfo);
		}
	}
	
}
void ALevelStreamingTrigger::OverlapEnds(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	FString screenSizeXStr = "Leaver1!!!";
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		FString screenSizeXStr = "Leaver2!!!";
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
		ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
		if (OtherActor == MyCharacter && LevelToLoad != "")
		{
			FString screenSizeXStr = "Leaver!!!";
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *screenSizeXStr);
			FLatentActionInfo LatentInfo;
			UGameplayStatics::UnloadStreamLevel(this, LevelToLoad, LatentInfo);
		}
	}
}



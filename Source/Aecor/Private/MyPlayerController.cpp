// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "Aecor.h"
#include "KeybindingUMG.h"



//General Log
DEFINE_LOG_CATEGORY(LogMyGame);

AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MyWidgetClassRef(TEXT("/Game/Maps/Dev/KeybindingWidget"));
	if (MyWidgetClassRef.Class) {
		KeybindingUIClass = MyWidgetClassRef.Class;
	}
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

			UMGIsOpen = false;
			/*if (!KeybindingUMG) //if KeybindingUMG isn't created & null
			{
					KeybindingUMG = CreateWidget<UUserWidget>(this, KeybindingUIClass);
					if (!KeybindingUMG) {
						return;
					}

					KeybindingUMG->AddToViewport(); //Add it to viewpport so the Construct() method in the UUserWidget:: is run
					//KeybindingUMG->SetVisibility(ESlateVisibility::Hidden); //Set it to hidden so it's not opened on spawn
			}*/

}
/*
void AMyPlayerController::Tick(float DeltaSeconds)
{
	if (IsInputKeyDown(EKeys::Escape)) {
		if (!UMGIsOpen) 
		{
			KeybindingUMG->SetVisibility(ESlateVisibility::Visible);
			UMGIsOpen = true;
		}
		else 
		{
			KeybindingUMG->SetVisibility(ESlateVisibility::Hidden);
			UMGIsOpen = false;
		}
	}
}*/
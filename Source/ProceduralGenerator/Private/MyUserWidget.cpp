// Fill out your copyright notice in the Description page of Project Settings.

//#include "MyUserWidget.h"
#include "../Public/MyUserWidget.h"

UMyUserWidget::UMyUserWidget(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer)
{
	initialAirEfficiency = 1.0f;
	initialHealth = 1.0f;
	initialHunger = 1.0f;
	initialThirst = 1.0f;

	

}

void UMyUserWidget::NativeConstruct()
{
	/*if (UCanvasPanel* OuterPanel = Cast<UCanvasPanel>(GetWidgetFromName("OuterPanel")))
	{
		backgroundBlur = NewObject<UBackgroundBlur>();
		OuterPanel->AddChild(backgroundBlur);
	}*/
}

float UMyUserWidget::CalculateAirEfficiencyPercentage()
{
	return initialAirEfficiency;
}

float UMyUserWidget::CalculateHealthPercentage()
{
	return initialHealth;
}

float UMyUserWidget::CalculateThirstPercentage()
{
	return initialThirst;
}

float UMyUserWidget::CalculateHungerPercentage()
{
	return initialHunger;
}

float UMyUserWidget::calculateRoundedAirEfficiency()
{
	int roundedAirEfficiency = FGenericPlatformMath::RoundToInt(delayedUpdateAirEfficiency * 100);

	if (roundedAirEfficiency < 0)
	{
		return 0;
	}
	else 
	{
		return roundedAirEfficiency;
	}
}



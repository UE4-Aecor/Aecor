// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/BackgroundBlur.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/Material.h"
#include "Components/Image.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "MyUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALGENERATOR_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMyUserWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "AirEfficiencyBar")
	float CalculateAirEfficiencyPercentage();

	UFUNCTION(BlueprintCallable, Category = "HealthPercentageBar")
	float CalculateHealthPercentage();

	UFUNCTION(BlueprintCallable, Category = "ThirstBar")
	float CalculateThirstPercentage();

	UFUNCTION(BlueprintCallable, Category = "HungerBar")
	float CalculateHungerPercentage();
	
	float initialAirEfficiency;

	float delayedUpdateAirEfficiency;

	float initialHealth;

	float initialHunger;

	float initialThirst;

	UBackgroundBlur* backgroundBlur;

	UPROPERTY(BlueprintReadWrite, Category = "ConvertedDynamicInstance")
	UMaterialInstanceDynamic* AirEfficiencyRadialBarInstance;

	UFUNCTION(BlueprintImplementableEvent, Category = "MyUserWidget")
	UImage* GetAirEfficiencyImage() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "MyUserWidget")
	UImage* GetHealthImage() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "MyUserWidget")
	UImage* GetHungerImage() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "MyUserWidget")
	UImage* GetThirstImage() const;

	UFUNCTION(BlueprintCallable, Category = "HungerBar")
	float calculateRoundedAirEfficiency();

};

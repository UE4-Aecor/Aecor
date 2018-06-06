// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/Actor.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
 //General Log
DECLARE_LOG_CATEGORY_EXTERN(LogMyGame, Log, All);

UCLASS()
class AECOR_API AMyPlayerController : public APlayerController
{
	//GENERATED_BODY()
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;

	//virtual void Tick(float DeltaSeconds) override;

	//The class that will be used for the keybinding UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player, HUD and UI")
		TSubclassOf<class UUserWidget> KeybindingUIClass;

	//The instance of the KeybindingUMG UI
		class UUserWidget* KeybindingUMG;

private:
	bool UMGIsOpen;
	
};

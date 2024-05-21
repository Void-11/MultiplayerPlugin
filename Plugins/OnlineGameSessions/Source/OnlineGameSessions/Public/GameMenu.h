// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameMenu.generated.h"

/**
 * 
 */
UCLASS()
class ONLINEGAMESESSIONS_API UGameMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable)
	void MenuSetup();
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DS1GameMode.generated.h"

/**
 * 
 */
UCLASS()
class DS1_API ADS1GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADS1GameMode();

// public:
// 	FORCEINLINE void SetGoStartMenu(bool InOnOff){bIsCurrentStartMenu = false;}

protected:
	virtual void BeginPlay() override;

// private:
// 	bool bIsCurrentStartMenu;
};

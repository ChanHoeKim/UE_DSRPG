// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/DS1GameMode.h"

#include "DS1GameInstance.h"
#include "Blueprint/UserWidget.h"

ADS1GameMode::ADS1GameMode()
{
}

void ADS1GameMode::BeginPlay()
{
	Super::BeginPlay();

	const FString CurrentMap = GetWorld()->GetMapName();

	if (CurrentMap.Contains(TEXT("Start")))
	{
		if (UDS1GameInstance* GI = Cast<UDS1GameInstance>(GetGameInstance()))
		{
			GI->MakeAndShowStartWidget();
		}
	}
}

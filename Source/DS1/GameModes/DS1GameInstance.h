// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DS1GameInstance.generated.h"

class UDS1StartMenuWidget;
/**
 * 
 */
UCLASS()
class DS1_API UDS1GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	void ShowStartWidget();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Start Menu Widget")
	TSubclassOf<UDS1StartMenuWidget> StartWidgetClass;
	
	UPROPERTY()
	UDS1StartMenuWidget* StartWidget;
};

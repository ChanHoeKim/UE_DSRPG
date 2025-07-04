// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DS1StatBarWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class DS1_API UDS1StatBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UProgressBar* HealthProgressBar;
	
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UProgressBar* StaminaProgressBar;

	

public:
	UDS1StatBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativePreConstruct() override;

	
public:
	void SetHpRatio(float Ratio) const;
	void SetStaminaRatio(float Ratio) const;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DS1EnemyHpBarWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class DS1_API UDS1EnemyHpBarWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UProgressBar* HealthProgressBar;

// private:
// 	UPROPERTY()
// 	FLinearColor FillColor;

public:
	UDS1EnemyHpBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativePreConstruct() override;

public:
	void SetHpRatio(float Ratio) const;
};

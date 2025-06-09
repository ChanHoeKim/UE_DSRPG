// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/DS1AttributeComponent.h"
#include "DS1BossHealthBarWidget.generated.h"

class UProgressBar;
class UDS1StatBarWidget;
/**
 * 
 */
UCLASS()
class DS1_API UDS1BossHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UProgressBar* BossHpProgressBar;

public:
	UDS1BossHealthBarWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativePreConstruct() override;

	
public:
	void Init(UDS1AttributeComponent* AttributeComponent);

protected:
	void OnAttributeChanged(EDS1AttributeType AttributeType, float InValue);

public:
	void SetHpRatio(float Ratio) const;
};

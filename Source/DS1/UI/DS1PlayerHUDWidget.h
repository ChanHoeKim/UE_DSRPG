// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DS1PlayerHUDWidget.generated.h"

enum class EDS1AttributeType : uint8;
class UDS1WeaponWidget;
class UDS1PotionWidget;
class UDS1StatBarWidget;
/**
 * 
 */
UCLASS()
class DS1_API UDS1PlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UDS1StatBarWidget* PlayerStat;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UDS1PotionWidget* PotionWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UDS1WeaponWidget* ShieldWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UDS1WeaponWidget* WeaponWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* BlankIcon;

public:
	UDS1PlayerHUDWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void NativeConstruct() override;

protected:
	
	void OnAttributeChanged(EDS1AttributeType AttributeType, float InValue);

	void OnPotionQuantityChanged(int32 InAmount);

	void OnWeaponChanged();
};

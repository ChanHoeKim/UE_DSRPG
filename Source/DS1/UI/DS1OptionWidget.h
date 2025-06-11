// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DS1OptionWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class DS1_API UDS1OptionWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	
public:
	UDS1OptionWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(meta=(BindWidget),BlueprintReadWrite)
	UButton* Decline_Button;

private:
	UFUNCTION()
	void DeclineButtonClick();
};

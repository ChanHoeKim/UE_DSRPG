// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DS1EscWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class DS1_API UDS1EscWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Resume_Button;

	// UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	// UButton* Option_Button;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Quit_Button;

public:
	UFUNCTION()
	void ResumeButtonClick();
	// UFUNCTION()
	// void OptionButtonClick();
	UFUNCTION()
	void QuitButtonClick();
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DS1StartMenuWidget.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class DS1_API UDS1StartMenuWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	
protected:
	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Start_Button;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Option_Button;

	UPROPERTY(meta=(BindWidget), BlueprintReadWrite)
	UButton* Quit_Button;

public:
	UFUNCTION()
	void StartButtonClick();
	
	UFUNCTION()
	void OptionButtonClick();
	
	UFUNCTION()
	void QuitButtonClick();
};

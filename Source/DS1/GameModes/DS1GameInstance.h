// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DS1GameInstance.generated.h"

class UDS1InputBlockingWidget;
class UDS1OptionWidget;
class UDS1PlayerHUDWidget;
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
	void MakeAndShowStartWidget();
	// FORCEINLINE UDS1PlayerHUDWidget* GetPlayerHUDWidget(){return PlayerHUDWidget;}
	//
	// FORCEINLINE bool GetbCurrentStartMenu(){return bCurrentStartMenu;}
	// FORCEINLINE void SetbCurrentStartMenu(bool InBool){bCurrentStartMenu=InBool;}
	//
	FORCEINLINE UDS1OptionWidget* GetOptionWidget() const {return OptionWidget;}
	FORCEINLINE UDS1InputBlockingWidget* GetInputBlockingWidget() const {return InputBlockingWidget;}
private:
	// bool bCurrentStartMenu = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Start Menu Widget")
	TSubclassOf<UDS1StartMenuWidget> StartWidgetClass;
	
	UPROPERTY()
	UDS1StartMenuWidget* StartWidget;
	
	// UPROPERTY(EditAnywhere, Category="HUD")
	// TSubclassOf<UUserWidget> PlayerHUDWidgetClass;
	//
	// UPROPERTY()
	// UDS1PlayerHUDWidget* PlayerHUDWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Option Widget")
	TSubclassOf<UUserWidget> OptionWidgetClass;

	UPROPERTY()
	UDS1OptionWidget* OptionWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input Blocking Widget")
	TSubclassOf<UUserWidget> InputBlockingWidgetClass;

	UPROPERTY()
	UDS1InputBlockingWidget* InputBlockingWidget;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Service/BTService_SelectBehavior.h"
#include "BTService_SelectBehaviorBoss.generated.h"

/**
 * 
 */
UCLASS()
class DS1_API UBTService_SelectBehaviorBoss : public UBTService_SelectBehavior
{
	GENERATED_BODY()

protected:
	/* Strafe 상태 : 공격을 수행하지 않고, 이동속도가 감소되며, Player를 락온 되어 좌우로 움직이는 상태 */
	/* Strafe 상태로 바뀌는 Stamina 값*/
	UPROPERTY(EditAnywhere)
	float StrafeActivationStaminaThreshold = 40.f;

protected:
	virtual void UpdateBehavior(UBlackboardComponent* BlackboardComp, ADS1Enemy* ControlledEnemy) const override;
};

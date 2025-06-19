// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "DS1EnemyAIController.generated.h"

class ADS1Enemy;
/**
 * 
 */
UCLASS()
class DS1_API ADS1EnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTreeAsset;

	/* AI가 주변을 인식(시각, 청각)할 수 있게 해주는 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPerceptionComponent* AIPerceptionComponent;

	// Controller가 빙의 후 UpdateTarget을 계속 호출 시키기 위한 Timer
 	FTimerHandle UpdateTargetTimerHandle;

	// Controller가 빙의한 대상
	UPROPERTY()
	ADS1Enemy* ControlledEnemy;

public:
	ADS1EnemyAIController();

public:
	void StopUpdateTarget();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	void UpdateTarget() const;
	void SetTarget(AActor* NewTarget) const;
};

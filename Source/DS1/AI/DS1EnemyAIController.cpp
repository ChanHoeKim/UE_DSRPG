// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/DS1EnemyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/DS1Character.h"
#include "Characters/DS1Enemy.h"
#include "Components/DS1RotationComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"

ADS1EnemyAIController::ADS1EnemyAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerception");
}

void ADS1EnemyAIController::StopUpdateTarget()
{
    GetWorld()->GetTimerManager().ClearTimer(UpdateTargetTimerHandle);
    SetTarget(nullptr);
}

void ADS1EnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

    ControlledEnemy = Cast<ADS1Enemy>(InPawn);

	RunBehaviorTree(BehaviorTreeAsset);

    // UpdateTarget 타이머 등록
    GetWorld()->GetTimerManager().SetTimer(UpdateTargetTimerHandle, this, &ThisClass::UpdateTarget, 0.1f, true);
}

void ADS1EnemyAIController::OnUnPossess()
{
    StopUpdateTarget();
    ControlledEnemy = nullptr;
	Super::OnUnPossess();
}

void ADS1EnemyAIController::UpdateTarget() const
{
    TArray<AActor*> OutActors;
    // 첫 번째 인자에 nullptr을 넣음으로서 Sight, Hearing, Damage 모두 인지 하도록 함
    AIPerceptionComponent->GetKnownPerceivedActors(nullptr, OutActors);

    // World에 존재하는 Player 0번인 Character를 가져옴 
    ADS1Character* PlayerCharacter = Cast<ADS1Character>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    //AI Perception Component에 Player가 감지되었는지 확인
    if (OutActors.Contains(PlayerCharacter))
    {
        //Player가 죽음 상태인지 체크
        if (!PlayerCharacter->IsDeath())
        {
            //BlackBoard에 등록
            SetTarget(PlayerCharacter);
            
            ControlledEnemy->ToggleHealthBarVisibility(true);
            ControlledEnemy->SeesTarget(PlayerCharacter);
        }
        else
        {
            SetTarget(nullptr);
            ControlledEnemy->ToggleHealthBarVisibility(false);
        }
    }
    else
    {
        SetTarget(nullptr);
        ControlledEnemy->ToggleHealthBarVisibility(false);
    }
}

void ADS1EnemyAIController::SetTarget(AActor* NewTarget) const
{
    if (IsValid(Blackboard))
    {
        Blackboard->SetValueAsObject(FName("Target"), NewTarget);
    }

    if (IsValid(ControlledEnemy))
    {
        if (UDS1RotationComponent* RotationComponent = ControlledEnemy->GetComponentByClass<UDS1RotationComponent>())
        {
            RotationComponent->SetTargetActor(NewTarget);
        }
    }
}

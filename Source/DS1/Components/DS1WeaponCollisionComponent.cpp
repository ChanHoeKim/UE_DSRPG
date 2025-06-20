// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DS1WeaponCollisionComponent.h"

#include "Kismet/KismetSystemLibrary.h"

UDS1WeaponCollisionComponent::UDS1WeaponCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
}

void UDS1WeaponCollisionComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UDS1WeaponCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsCollisionEnabled)
	{
		CollisionTrace();
	}
}

void UDS1WeaponCollisionComponent::TurnOnCollision()
{
	AlreadyHitActors.Empty();
	bIsCollisionEnabled = true;
}

void UDS1WeaponCollisionComponent::TurnOffCollision()
{
	bIsCollisionEnabled = false;
}

void UDS1WeaponCollisionComponent::SetWeaponMesh(UPrimitiveComponent* MeshComponent)
{
	WeaponMesh = MeshComponent;
}

void UDS1WeaponCollisionComponent::AddIgnoredActor(AActor* Actor)
{
	IgnoredActors.Add(Actor);
}

void UDS1WeaponCollisionComponent::RemoveIgnoredActor(AActor* Actor)
{
	IgnoredActors.Remove(Actor);
}

bool UDS1WeaponCollisionComponent::CanHitActor(AActor* Actor) const
{
	return AlreadyHitActors.Contains(Actor) == false;
}

// Collision Trace 생성
void UDS1WeaponCollisionComponent::CollisionTrace()
{
    TArray<FHitResult> OutHits;

    const FVector Start = WeaponMesh->GetSocketLocation(TraceStartSocketName);
    const FVector End = WeaponMesh->GetSocketLocation(TraceEndSocketName);

    bool const bHit = UKismetSystemLibrary::SphereTraceMultiForObjects(
        GetOwner(),
        Start, // Sphere Trace의 시작 위치
        End, //       "      의 끝 위치
        TraceRadius, // 반지름
        TraceObjectTypes, // 생성자에서 Pawn만 감지하도록 추가
        false, // 복잡한 콜리전 미사용
        IgnoredActors, // 무시할 액터
        //DrawDebugType,
        EDrawDebugTrace::None, // 화면에 표출되는 방식
        OutHits, //                       감지된 결과
        true // 자신 무시할지 여부
        );

    if (bHit)
    {
        for (const FHitResult& Hit : OutHits)
        {
            AActor* HitActor = Hit.GetActor();

            if (HitActor == nullptr)
            {
                continue;
            }

        	// 한 번 공격에 동일한 대상이 여러 번 Hit 되지 않도록 체크 
            if (CanHitActor(HitActor))
            {
                AlreadyHitActors.Add(HitActor);
            	
                if (OnHitActor.IsBound())
                {
                    OnHitActor.Broadcast(Hit);
                }
            }
        }
    }
}


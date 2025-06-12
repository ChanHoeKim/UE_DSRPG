// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DS1WeaponCollisionComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FOnHitActor, const FHitResult&);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), meta=(DisplayName = "Weapon Collision") )
class DS1_API UDS1WeaponCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Actor가 Hit 되었을 때 호출될 델리게이트
	FOnHitActor OnHitActor;

/* 무기 충돌 처리를 위해 Sphere Trace를 씌울 위치 지정 */
protected:
	/* 무기에 씌운 Sphere Trace의 시작 지점 소켓 */
	UPROPERTY(EditAnywhere)
	FName TraceStartSocketName;

	/* 무기에 씌운 Sphere Trace의 끝 지점 소켓 */
	UPROPERTY(EditAnywhere)
	FName TraceEndSocketName;

protected:
	/* Sphere 크기(반지름) */
	UPROPERTY(EditAnywhere)
	float TraceRadius = 20.f;

	/* 감지 대상 = 생성자에서 Pawn만 감지하도록 Add 처리 */
	UPROPERTY(EditAnywhere)
	TArray <TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;

	/* 충돌처리 제외 Actors */
	UPROPERTY(EditAnywhere)
	TArray<AActor*> IgnoredActors;

	/* Collision(Sphere)이 화면에 표출되는 형태 */
	// ForDuration = Sphere가 일정 시간동안 화면에 나옴(기본 5초)
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EDrawDebugTrace::Type> DrawDebugType = EDrawDebugTrace::ForDuration;

protected:
	/* 무기의 MeshComponent */
	// StaticMesh, SkeletalMesh 등
	UPROPERTY()
	UPrimitiveComponent* WeaponMesh;

	// Hit 처리한 Actor를 담는 변수
	// 한 번 공격에 동일한 대상이 중복 Hit 되지 않기위한 변수
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

	bool bIsCollisionEnabled = false;

public:	
	UDS1WeaponCollisionComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void TurnOnCollision();

	void TurnOffCollision();

	void SetWeaponMesh(UPrimitiveComponent* MeshComponent);

	void AddIgnoredActor(AActor* Actor);

	void RemoveIgnoredActor(AActor* Actor);

protected:
	bool CanHitActor(AActor* Actor) const;

	void CollisionTrace();
	
};

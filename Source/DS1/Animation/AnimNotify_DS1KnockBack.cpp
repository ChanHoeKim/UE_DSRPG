// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_DS1KnockBack.h"

#include "DS1GameplayTags.h"
#include "GameplayTagContainer.h"
#include "Characters/DS1Character.h"
#include "Components/DS1StateComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UAnimNotify_DS1KnockBack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const FVector OriginLocation = MeshComp->GetSocketLocation(LocationSocketName);
	TArray<AActor*> OverlappedActors;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(MeshComp->GetOwner());
	
	UKismetSystemLibrary::DrawDebugSphere(MeshComp, OriginLocation, DamageRadius, 12, FLinearColor::White, 2.f, 1.f);
	bool bCheck = UKismetSystemLibrary::SphereOverlapActors(
		MeshComp,
		OriginLocation,
		DamageRadius,
		ObjectTypes,
		AActor::StaticClass(),
		ActorsToIgnore,
		OverlappedActors);

	bool TagCheck = true;
	FGameplayTagContainer CheckTags;
	CheckTags.AddTag(DS1GameplayTags::Character_State_Rolling);

	//;
	
	for (AActor* Actor : OverlappedActors)
	{
		if (!Actor) continue;
		
		if (ADS1Character* Character = Cast<ADS1Character>(Actor))
		{
			TagCheck = Character->GetStateComponent()->IsCurrentStateEqualToAny(CheckTags);

			if (bCheck && TagCheck == false)
			{
				// if (GEngine)
				// {
				// 	GEngine->AddOnScreenDebugMessage(
				// 		/* Key */ -1,
				// 		/* Duration */ 5.0f,
				// 		/* Color */ FColor::Green,
				// 		/* Message */ TEXT("맞음")
				// 	);
				// }
				Character->NockDown(MeshComp->GetOwner());
			}
		}
	}
	
	

	APawn* OwnerPawn = Cast<APawn>(MeshComp->GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	AController* OwnerController = OwnerPawn->GetController();
	if (!OwnerController)
	{
		return;
	}
	

	UGameplayStatics::ApplyRadialDamage(OwnerPawn, BaseDamage, OriginLocation, DamageRadius, nullptr, ActorsToIgnore, nullptr, OwnerController, false, ECC_Visibility);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/DS1MontageActionData.h"

//착용하고 있는 무기 클래스에 해당하는 Data Asset을 가져오고,
//해당 Data Asset에서 배열로 저장한 Montage 중 특정 Montage를 찾음
UAnimMontage* UDS1MontageActionData::GetMontageForTag(const FGameplayTag& GroupTag, const int32 Index) const
{
	if (MontageGroupMap.Contains(GroupTag))
	{
		const FDS1MontageGroup& MontageGroup = MontageGroupMap[GroupTag];

		if (MontageGroup.Animations.Num() > 0 && MontageGroup.Animations.Num() > Index)
		{
			return MontageGroup.Animations[Index];
		}
	}

	return nullptr;
}

// Enemy 캐릭터의 다양한 공격을 위한 공격 랜덤 처리 함수
UAnimMontage* UDS1MontageActionData::GetRandomMontageForTag(const FGameplayTag& GroupTag) const
{
	if (MontageGroupMap.Contains(GroupTag))
	{
		const FDS1MontageGroup& MontageGroup = MontageGroupMap[GroupTag];

		// 랜덤 Index
		const int32 RandomIndex = FMath::RandRange(0, MontageGroup.Animations.Num() - 1);

		return GetMontageForTag(GroupTag, RandomIndex);
	}

	return nullptr;
}

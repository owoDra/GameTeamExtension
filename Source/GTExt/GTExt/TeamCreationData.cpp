// Copyright (C) 2023 owoDra

#include "TeamCreationData.h"

#include "Info/TeamInfo_Private.h"
#include "Info/TeamInfo_Public.h"
#include "Assign/TeamAssignBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamCreationData)


UTeamCreationData::UTeamCreationData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PublicTeamInfoClass = ATeamInfo_Public::StaticClass();
	PrivateTeamInfoClass = ATeamInfo_Private::StaticClass();

	TeamAssignType = ObjectInitializer.CreateDefaultSubobject<UTeamAssignBase>(this, FName(TEXT("TeamAssignType")));
}
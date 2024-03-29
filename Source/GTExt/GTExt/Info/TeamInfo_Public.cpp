﻿// Copyright (C) 2024 owoDra

#include "TeamInfo_Public.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamInfo_Public)


ATeamInfo_Public::ATeamInfo_Public(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ATeamInfo_Public::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_InitialOnly;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS_FAST(ATeamInfo_Public, TeamDisplayData, Params);
}


void ATeamInfo_Public::SetTeamDisplayData(TObjectPtr<UTeamDisplayData> NewDisplayData)
{
	check(HasAuthority());
	check(NewDisplayData);

	TeamDisplayData = NewDisplayData;
	MARK_PROPERTY_DIRTY_FROM_NAME(ATeamInfo_Public, TeamDisplayData, this);

	TryRegisterWithTeamManagerSubsystem();
}

void ATeamInfo_Public::OnRep_TeamDisplayData()
{
	TryRegisterWithTeamManagerSubsystem();
}

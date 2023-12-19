// Copyright (C) 2023 owoDra

#include "TeamInfo_Public.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamInfo_Public)


ATeamInfo_Public::ATeamInfo_Public(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ATeamInfo_Public::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, TeamDisplayData, COND_InitialOnly);
}


void ATeamInfo_Public::SetTeamDisplayData(TObjectPtr<UTeamDisplayData> NewDisplayData)
{
	check(HasAuthority());
	check(NewDisplayData);

	TeamDisplayData = NewDisplayData;

	TryRegisterWithTeamManagerSubsystem();
}

void ATeamInfo_Public::OnRep_TeamDisplayData()
{
	TryRegisterWithTeamManagerSubsystem();
}

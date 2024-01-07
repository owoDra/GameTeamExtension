// Copyright (C) 2023 owoDra

#include "TeamInfoBase.h"

#include "TeamManagerSubsystem.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamInfoBase)


ATeamInfoBase::ATeamInfoBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamTags(this)
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.0f;
	SetReplicatingMovement(false);
}

void ATeamInfoBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, TeamTags);
	DOREPLIFETIME_CONDITION(ThisClass, TeamId, COND_InitialOnly);
}


void ATeamInfoBase::BeginPlay()
{
	Super::BeginPlay();

	TryRegisterWithTeamManagerSubsystem();
}

void ATeamInfoBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (TeamId != INDEX_NONE)
	{
		auto* TMS{ GetWorld()->GetSubsystem<UTeamManagerSubsystem>() };
		TMS->UnregisterTeamInfo(this);
	}

	Super::EndPlay(EndPlayReason);
}


void ATeamInfoBase::TryRegisterWithTeamManagerSubsystem()
{
	if (TeamId != INDEX_NONE)
	{
		auto* TMS{ GetWorld()->GetSubsystem<UTeamManagerSubsystem>() };
		RegisterWithTeamManagerSubsystem(TMS);
	}
}

void ATeamInfoBase::RegisterWithTeamManagerSubsystem(UTeamManagerSubsystem* Subsystem)
{
	Subsystem->RegisterTeamInfo(this);
}


void ATeamInfoBase::SetTeamId(int32 NewTeamId)
{
	check(HasAuthority());
	check(TeamId == INDEX_NONE);
	check(NewTeamId != INDEX_NONE);

	TeamId = NewTeamId;

	TryRegisterWithTeamManagerSubsystem();
}

void ATeamInfoBase::OnRep_TeamId()
{
	TryRegisterWithTeamManagerSubsystem();
}

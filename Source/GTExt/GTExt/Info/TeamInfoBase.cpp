// Copyright (C) 2024 owoDra

#include "TeamInfoBase.h"

#include "TeamManagerSubsystem.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

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

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	Params.Condition = COND_InitialOnly;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS_FAST(ATeamInfoBase, TeamId, Params);

	Params.Condition = COND_None;
	Params.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS_FAST(ATeamInfoBase, TeamTags, Params);
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
	MARK_PROPERTY_DIRTY_FROM_NAME(ATeamInfoBase, TeamId, this);

	TryRegisterWithTeamManagerSubsystem();
}

void ATeamInfoBase::OnRep_TeamId()
{
	TryRegisterWithTeamManagerSubsystem();
}

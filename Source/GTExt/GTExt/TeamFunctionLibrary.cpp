// Copyright (C) 2023 owoDra

#include "TeamFunctionLibrary.h"

#include "TeamMemberComponent.h"
#include "TeamMemberComponentInterface.h"

#include "GenericTeamAgentInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamFunctionLibrary)


int32 UTeamFunctionLibrary::GenericTeamIdToInteger(FGenericTeamId ID)
{
	return (ID == FGenericTeamId::NoTeam) ? INDEX_NONE : (int32)ID;
}

FGenericTeamId UTeamFunctionLibrary::IntegerToGenericTeamId(int32 ID)
{
	return (ID == INDEX_NONE) ? FGenericTeamId::NoTeam : FGenericTeamId((uint8)ID);
}


UTeamMemberComponent* UTeamFunctionLibrary::GetTeamMemberComponentFromActor(const AActor* Actor, bool LookForComponent)
{
	if (!Actor)
	{
		return nullptr;
	}

	const auto* TMCI{ Cast<ITeamMemberComponentInterface>(Actor) };
	if (TMCI)
	{
		return TMCI->GetTeamMemberComponent();
	}

	if (LookForComponent)
	{
		return UTeamMemberComponent::FindTeamMemberComponent(Actor);
	}

	return nullptr;
}


﻿// Copyright (C) 2024 owoDra

#include "TeamManagerSubsystem.h"

#include "Info/TeamInfo_Private.h"
#include "Info/TeamInfo_Public.h"
#include "Assign/TeamAssignBase.h"
#include "TeamManagerComponent.h"
#include "TeamMemberComponent.h"
#include "TeamFunctionLibrary.h"
#include "TeamCreationData.h"
#include "GTExtLogs.h"

#include "GenericTeamAgentInterface.h"
#include "GameFramework/GameStateBase.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamManagerSubsystem)


void UTeamManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UTeamManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}


void UTeamManagerSubsystem::RegisterTeamInfo(ATeamInfoBase* TeamInfo)
{
	check(TeamInfo);

	const auto TeamId{ TeamInfo->GetTeamId() };
	check(TeamId != INDEX_NONE);

	auto& Entry{ TeamMap.FindOrAdd(TeamId) };
	Entry.SetTeamInfo(TeamInfo);
}

void UTeamManagerSubsystem::UnregisterTeamInfo(ATeamInfoBase* TeamInfo)
{
	check(TeamInfo);

	const auto TeamId{ TeamInfo->GetTeamId() };
	check(TeamId != INDEX_NONE);

	auto& Entry{ TeamMap.FindChecked(TeamId) };
	Entry.RemoveTeamInfo(TeamInfo);
}

void UTeamManagerSubsystem::NotifyTeamDisplayDataModified(UTeamDisplayData* ModifiedData)
{
	for (const auto& KVP : TeamMap)
	{
		const auto& TeamId{ KVP.Key };
		const auto& TrackingInfo{ KVP.Value };

		TrackingInfo.OnTeamDisplayDataChanged.Broadcast(TrackingInfo.DisplayData);
	}
}


bool UTeamManagerSubsystem::ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamId)
{
	const auto NewTeamID{ UTeamFunctionLibrary::IntegerToGenericTeamId(NewTeamId) };

	if (auto* TMC{ UTeamFunctionLibrary::GetTeamMemberComponentFromActor(ActorToChange) })
	{
		TMC->SetGenericTeamId(NewTeamID);

		return true;
	}

	return false;
}

bool UTeamManagerSubsystem::CanCauseDamage(const AActor* Instigator, const AActor* Target, bool bAllowDamageToSelf) const
{
	// Whether or not you can do damage to yourself.

	if (bAllowDamageToSelf && (Instigator == Target))
	{
		return true;
	}

	// Whether both teams are different

	int32 InstigatorTeamId;
	int32 TargetTeamId;
	const auto Relationship{ CompareTeams(Instigator, Target, /*out*/ InstigatorTeamId, /*out*/ TargetTeamId) };

	if (Relationship == ETeamComparison::DifferentTeams)
	{
		return true;
	}

	return false;
}

ETeamComparison UTeamManagerSubsystem::CompareTeams(const AActor* A, const AActor* B, int32& TeamIdA, int32& TeamIdB) const
{
	if (A == B)
	{
		TeamIdA = INDEX_NONE;
		TeamIdB = INDEX_NONE;

		return ETeamComparison::OnSameTeam;
	}

	TeamIdA = FindTeamFromActor(A);
	TeamIdB = FindTeamFromActor(B);

	if ((TeamIdA == INDEX_NONE) || (TeamIdB == INDEX_NONE))
	{
		return ETeamComparison::InvalidArgument;
	}
	else
	{
		return (TeamIdA == TeamIdB) ? ETeamComparison::OnSameTeam : ETeamComparison::DifferentTeams;
	}
}

ETeamComparison UTeamManagerSubsystem::CompareTeams(const AActor* A, const AActor* B) const
{
	int32 TeamIdA;
	int32 TeamIdB;

	return CompareTeams(A, B, /*out*/ TeamIdA, /*out*/ TeamIdB);
}

void UTeamManagerSubsystem::AddTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount)
{
	auto FailureHandler
	{
		[&](const FString& ErrorMessage)
		{
			UE_LOG(LogGameExt_Team, Error, TEXT("AddTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);
		}
	};

	if (auto* Entry{ TeamMap.Find(TeamId) })
	{
		if (Entry->PublicInfo)
		{
			if (Entry->PublicInfo->HasAuthority())
			{
				Entry->PublicInfo->TeamTags.AddStack(Tag, StackCount);
			}
			else
			{
				FailureHandler(TEXT("failed because it was called on a client"));
			}
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

void UTeamManagerSubsystem::RemoveTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount)
{
	auto FailureHandler
	{
		[&](const FString& ErrorMessage)
		{
			UE_LOG(LogGameExt_Team, Error, TEXT("RemoveTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);
		}
	};

	if (auto* Entry{ TeamMap.Find(TeamId) })
	{
		if (Entry->PublicInfo)
		{
			if (Entry->PublicInfo->HasAuthority())
			{
				Entry->PublicInfo->TeamTags.RemoveStack(Tag, StackCount);
			}
			else
			{
				FailureHandler(TEXT("failed because it was called on a client"));
			}
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

void UTeamManagerSubsystem::SetTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount)
{
	auto FailureHandler
	{
		[&](const FString& ErrorMessage)
		{
			UE_LOG(LogGameExt_Team, Error, TEXT("SetTeamTagStack(TeamId: %d, Tag: %s, StackCount: %d) %s"), TeamId, *Tag.ToString(), StackCount, *ErrorMessage);
		}
	};

	if (auto* Entry{ TeamMap.Find(TeamId) })
	{
		if (Entry->PublicInfo)
		{
			if (Entry->PublicInfo->HasAuthority())
			{
				Entry->PublicInfo->TeamTags.SetStack(Tag, StackCount);
			}
			else
			{
				FailureHandler(TEXT("failed because it was called on a client"));
			}
		}
		else
		{
			FailureHandler(TEXT("failed because there is no team info spawned yet (called too early, before the experience was ready)"));
		}
	}
	else
	{
		FailureHandler(TEXT("failed because it was passed an unknown team id"));
	}
}

int32 UTeamManagerSubsystem::GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const
{
	if (const auto* Entry{ TeamMap.Find(TeamId) })
	{
		const auto PublicStackCount{ (Entry->PublicInfo != nullptr) ? Entry->PublicInfo->TeamTags.GetStackCount(Tag) : 0 };
		const auto PrivateStackCount{ (Entry->PrivateInfo != nullptr) ? Entry->PrivateInfo->TeamTags.GetStackCount(Tag) : 0 };

		return PublicStackCount + PrivateStackCount;
	}
	else
	{
		UE_LOG(LogGameExt_Team, Log, TEXT("GetTeamTagStackCount(TeamId: %d, Tag: %s) failed because it was passed an unknown team id"), TeamId, *Tag.ToString());
		
		return 0;
	}
}

bool UTeamManagerSubsystem::TeamHasTag(int32 TeamId, FGameplayTag Tag) const
{
	return GetTeamTagStackCount(TeamId, Tag) > 0;
}

bool UTeamManagerSubsystem::DoesTeamExist(int32 TeamId) const
{
	return TeamMap.Contains(TeamId);
}


FTeamDisplayDataChangedDelegate& UTeamManagerSubsystem::GetTeamDisplayDataChangedDelegate(int32 TeamId)
{
	return TeamMap.FindOrAdd(TeamId).OnTeamDisplayDataChanged;
}

int32 UTeamManagerSubsystem::FindTeamFromActor(const AActor* TestActor) const
{
	if (auto* TMC{ UTeamFunctionLibrary::GetTeamMemberComponentFromActor(TestActor) })
	{
		return UTeamFunctionLibrary::GenericTeamIdToInteger(TMC->GetGenericTeamId());
	}

	return INDEX_NONE;
}

void UTeamManagerSubsystem::BP_FindTeamFromActor(const AActor* TestActor, bool& bIsPartOfTeam, int32& TeamId) const
{
	TeamId = FindTeamFromActor(TestActor);
	bIsPartOfTeam = TeamId != INDEX_NONE;
}

UTeamDisplayData* UTeamManagerSubsystem::GetTeamDisplayData(int32 TeamId, int32 ViewerTeamId)
{
	if (auto * Entry{ TeamMap.Find(TeamId) })
	{
		return Entry->DisplayData;
	}

	return nullptr;
}

UTeamDisplayData* UTeamManagerSubsystem::GetEffectiveTeamDisplayData(int32 TeamId, AActor* ViewerTeamAgent)
{
	return GetTeamDisplayData(TeamId, FindTeamFromActor(ViewerTeamAgent));
}

TArray<int32> UTeamManagerSubsystem::GetTeamIDs() const
{
	TArray<int32> Result;
	TeamMap.GenerateKeyArray(Result);
	Result.Sort();

	return Result;
}

TArray<int32> UTeamManagerSubsystem::GetEnemyTeamIDsFromActor(const AActor* TestActor) const
{
	TArray<int32> Result;
	TeamMap.GenerateKeyArray(Result);

	auto AlliesTeamId{ FindTeamFromActor(TestActor) };
	if (AlliesTeamId == INDEX_NONE)
	{
		Result.Sort();

		return Result;
	}

	Result.RemoveSwap(AlliesTeamId);

	Result.Sort();

	return Result;
}


// Game Mode Option

bool UTeamManagerSubsystem::InitializeFromGameModeOption()
{
	auto bResult{ false };

	UE_LOG(LogGameExt_Team, Log, TEXT("Initialize Team Stat From Game Mode Option"));

	for (const auto& KVP : TeamMap)
	{
		if (auto PublicTeamInfo{ KVP.Value.PublicInfo })
		{
			bResult |= PublicTeamInfo->InitializeFromGameModeOption();
		}
	}

	return bResult;
}

FString UTeamManagerSubsystem::ConstructGameModeOption() const
{
	FString Options;

	// Create Option from TeamInfo

	for (const auto& KVP : TeamMap)
	{
		if (auto PublicTeamInfo{ KVP.Value.PublicInfo })
		{
			Options += PublicTeamInfo->ConstructGameModeOption();
		}
	}

	// Create Option from TeamAssign

	const auto* World{ GetWorld() };
	const auto* GameState{ World ? World->GetGameState() : nullptr };
	const auto* TeamManager{ GameState ? GameState->FindComponentByClass<UTeamManagerComponent>() : nullptr };
	const auto* TeamCreationData{ TeamManager ? TeamManager->GetTeamCreationData() : nullptr };
	const auto TeamAssgin{ TeamCreationData ? TeamCreationData->TeamAssignType : nullptr };

	if (ensure(TeamAssgin))
	{
		Options += TeamAssgin->ConstructGameModeOption(GameState->PlayerArray);
	}

	return Options;
}

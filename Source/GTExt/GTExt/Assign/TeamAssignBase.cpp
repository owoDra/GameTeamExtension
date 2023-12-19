// Copyright (C) 2023 owoDra

#include "TeamAssignBase.h"

#include "TeamFunctionLibrary.h"
#include "TeamCreationData.h"
#include "TeamMemberComponent.h"

#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamAssignBase)


UTeamAssignBase::UTeamAssignBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UTeamAssignBase::AssignTeamForPlayer(const UTeamCreationData* TeamCreationData, APlayerState* PlayerState, AGameStateBase* GameState) const
{
	auto* TMC{ UTeamFunctionLibrary::GetTeamMemberComponentFromActor(PlayerState) };

	const auto& TeamsToCreate{ TeamCreationData->TeamsToCreate };
	const auto NumTeams{ TeamsToCreate.Num() };

	if (NumTeams > 0)
	{
		// Create a map to count the number of players on a team
		
		TMap<int32, uint32> TeamMemberCounts;
		TeamMemberCounts.Reserve(NumTeams);

		for (const auto& KVP : TeamsToCreate)
		{
			TeamMemberCounts.Add(KVP.Key, 0);
		}

		// Count the number of players on the team

		for (const auto& PS : GameState->PlayerArray)
		{
			if (auto* OtherTMC{ UTeamFunctionLibrary::GetTeamMemberComponentFromActor(PS) })
			{
				const auto PlayerTeamID{ OtherTMC->GetGenericTeamId().GetId() };

				// do not count unassigned or disconnected players

				if (!PS->IsInactive())	
				{
					if (TeamMemberCounts.Contains(PlayerTeamID))
					{
						TeamMemberCounts[PlayerTeamID] += 1;
					}
				}
			}
		}

		// Sort by lowest team population, then by team ID

		auto BestTeamId{ static_cast<int32>(INDEX_NONE) };
		auto BestPlayerCount{ TNumericLimits<uint32>::Max() };

		for (const auto& KVP : TeamMemberCounts)
		{
			const auto& TestTeamId{ KVP.Key };
			const auto& TestTeamPlayerCount{ KVP.Value };

			if (TestTeamPlayerCount < BestPlayerCount)
			{
				BestTeamId = TestTeamId;
				BestPlayerCount = TestTeamPlayerCount;
			}
			else if (TestTeamPlayerCount == BestPlayerCount)
			{
				if ((TestTeamId < BestTeamId) || (BestTeamId == INDEX_NONE))
				{
					BestTeamId = TestTeamId;
					BestPlayerCount = TestTeamPlayerCount;
				}
			}
		}

		TMC->SetGenericTeamId(UTeamFunctionLibrary::IntegerToGenericTeamId(BestTeamId));
	}
	else
	{
		TMC->SetGenericTeamId(FGenericTeamId::NoTeam);
	}
}

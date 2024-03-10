// Copyright (C) 2024 owoDra

#include "TeamInfoBase.h"

#include "TeamManagerSubsystem.h"
#include "GTExtLogs.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagsManager.h"

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


// Game Mode Option

bool ATeamInfoBase::InitializeFromGameModeOption()
{
	auto* GameMode{ GetWorld()->GetAuthGameMode() };
	if (ensure(GameMode))
	{
		auto OptionString{ GameMode->OptionsString };
		const auto OptionKey{ FString::Printf(TEXT("Team[%d]"), TeamId) };

		UE_LOG(LogGameExt_Team, Log, TEXT("| Team[%d]"), TeamId);

		// Retrieve information about the team corresponding to this TeamInfo from the game mode options

		if (UGameplayStatics::HasOption(OptionString, OptionKey))
		{
			auto OptionValue{ UGameplayStatics::ParseOption(OptionString, OptionKey) };
			
			UE_LOG(LogGameExt_Team, Log, TEXT("| | OptionValue: %s"), *OptionValue);

			// Query StatTag information about TeamInfo

			TArray<FString> Stats;
			OptionValue.ParseIntoArray(Stats, TEXT(":"));
			for (const auto& Stat : Stats)
			{
				// Query the value of StatTag

				TArray<FString> Values;
				Stat.ParseIntoArray(Values, TEXT(","));

				// Check if the value is correct.

				static constexpr int32 NumValues{ 3 };

				if (ensure(Values.Num() == NumValues))
				{
					UE_LOG(LogGameExt_Team, Log, TEXT("| | | Tag: %s"), *Values[0]);
					UE_LOG(LogGameExt_Team, Log, TEXT("| | | Val: %s"), *Values[1]);
					UE_LOG(LogGameExt_Team, Log, TEXT("| | | Max: %s"), *Values[2]);

					const auto Tag{ UGameplayTagsManager::Get().RequestGameplayTag(FName(Values[0]), true) };

					TeamTags.SetMaxStack(Tag, FCString::Atoi(*Values[2]));
					TeamTags.SetStack(Tag, FCString::Atoi(*Values[1]));
				}
			}
		}
		else
		{
			UE_LOG(LogGameExt_Team, Log, TEXT("| | No Options"))
		}

		return true;
	}

	return false;
}

FString ATeamInfoBase::ConstructGameModeOption() const
{
	FString ScoreOption;

	for (const auto& KVP : TeamTags.FastStacks)
	{
		const auto Tag{ KVP.Key };
		const auto Val{ KVP.Value.StackCount };
		const auto Max{ KVP.Value.MaxStackCount };

		ScoreOption += FString::Printf(TEXT("%s,%d,%d:"), *Tag.ToString(), Val, Max);
	}

	return FString::Printf(TEXT("?Team[%d]=%s"), TeamId, *ScoreOption);
}

// Copyright (C) 2023 owoDra

#include "TeamManagerComponent.h"

#include "TeamFunctionLibrary.h"
#include "TeamManagerSubsystem.h"
#include "TeamMemberComponent.h"
#include "TeamCreationData.h"
#include "Info/TeamInfo_Private.h"
#include "Info/TeamInfo_Public.h"
#include "Assign/TeamAssignBase.h"
#include "GTExtLogs.h"

#include "Actor/GFCGameMode.h"
#include "InitState/InitStateTags.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamManagerComponent)


const FName UTeamManagerComponent::NAME_ActorFeatureName("TeamManager");

UTeamManagerComponent::UTeamManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UTeamManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTeamManagerComponent, TeamCreationData);
}


void UTeamManagerComponent::OnRegister()
{
	Super::OnRegister();

	// This component can only be added to classes derived from AGameState

	const auto* GameState{ GetGameState<AGameStateBase>() };
	ensureAlwaysMsgf((GameState != nullptr), TEXT("TeamManagerComponent on [%s] can only be added to GameState."), *GetNameSafe(GetOwner()));

	// No more than two of these components should be added to a Pawn.

	TArray<UActorComponent*> Components;
	GetOwner()->GetComponents(UTeamManagerComponent::StaticClass(), Components);
	ensureAlwaysMsgf((Components.Num() == 1), TEXT("Only one TeamManagerComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register this component in the GameFrameworkComponentManager.

	RegisterInitStateFeature();
}

void UTeamManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// Start listening for changes in the initialization state of all features 
	// related to the Pawn that owns this component.

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Change the initialization state of this component to [Spawned]

	ensure(TryToChangeInitState(TAG_InitState_Spawned));

	// Check if initialization process can continue

	CheckDefaultInitialization();
}

void UTeamManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();

	Super::EndPlay(EndPlayReason);
}


bool UTeamManagerComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	check(Manager);

	auto* Owner{ GetOwner() };

	/**
	 * [None] -> [Spawned]
	 */
	if (!CurrentState.IsValid() && DesiredState == TAG_InitState_Spawned)
	{
		// Check Owner

		if (Owner)
		{
			return true;
		}
	}

	/**
	 * [Spawned] -> [DataAvailable]
	 */
	else if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		// Check Team Creation Data

		if (TeamCreationData)
		{
			return true;
		}
	}

	/**
	 * [DataAvailable] -> [DataInitialized]
	 */
	else if (CurrentState == TAG_InitState_DataAvailable && DesiredState == TAG_InitState_DataInitialized)
	{
		return true;
	}

	/**
	 * [DataInitialized] -> [GameplayReady]
	 */
	else if (CurrentState == TAG_InitState_DataInitialized && DesiredState == TAG_InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UTeamManagerComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	UE_LOG(LogGTE, Log, TEXT("[%s] Team Manager Component InitState Reached: %s"),
		GetOwner()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"), *DesiredState.GetTagName().ToString());

	/**
	 * [Spawned] -> [DataAvailable]
	 */
	if (CurrentState == TAG_InitState_Spawned && DesiredState == TAG_InitState_DataAvailable)
	{
		ApplyTeamCreationData();
	}
}

void UTeamManagerComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
}

void UTeamManagerComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain
	{
		TAG_InitState_Spawned,
		TAG_InitState_DataAvailable,
		TAG_InitState_DataInitialized,
		TAG_InitState_GameplayReady
	};

	ContinueInitStateChain(StateChain);
}


void UTeamManagerComponent::OnRep_TeamCreationData()
{
	CheckDefaultInitialization();
}

void UTeamManagerComponent::ApplyTeamCreationData()
{
	check(TeamCreationData);

	if (GetOwner()->HasAuthority())
	{
		ServerCreateTeams();
		ServerAssignPlayersToTeams();
	}
}

void UTeamManagerComponent::ServerCreateTeams()
{
	auto* World{ GetWorld() };
	check(World);

	auto* TMS{ World->GetSubsystem<UTeamManagerSubsystem>() };

	const auto& PublicTeamInfoClass{ TeamCreationData->PublicTeamInfoClass };
	check(PublicTeamInfoClass);

	const auto& PrivateTeamInfoClass{ TeamCreationData->PrivateTeamInfoClass };
	check(PrivateTeamInfoClass);

	for (const auto& KVP : TeamCreationData->TeamsToCreate)
	{
		// Create only if it does not already exist

		if (!TMS->DoesTeamExist(KVP.Key))
		{
			const auto TeamId{ static_cast<int32>(KVP.Key) };
			const auto& DisplayData{ KVP.Value };

			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			auto* NewTeamPublicInfo{ World->SpawnActor<ATeamInfo_Public>(PublicTeamInfoClass, SpawnInfo) };
			checkf(NewTeamPublicInfo != nullptr, TEXT("Failed to create public team actor from class %s"), *GetPathNameSafe(*PublicTeamInfoClass));
			NewTeamPublicInfo->SetTeamId(TeamId);
			NewTeamPublicInfo->SetTeamDisplayData(DisplayData);

			auto* NewTeamPrivateInfo{ World->SpawnActor<ATeamInfo_Private>(PrivateTeamInfoClass, SpawnInfo) };
			checkf(NewTeamPrivateInfo != nullptr, TEXT("Failed to create private team actor from class %s"), *GetPathNameSafe(*PrivateTeamInfoClass));
			NewTeamPrivateInfo->SetTeamId(TeamId);
		}
	}
}

void UTeamManagerComponent::ServerAssignPlayersToTeams()
{
	auto* GameState{ GetGameStateChecked<AGameStateBase>() };

	// Assign players that already exist to teams

	for (const auto& PlayerState : GameState->PlayerArray)
	{
		ServerChooseTeamForPlayer(PlayerState);
	}

	// Listen for new players logging in

	if (auto* GFCGameMode{ Cast<AGFCGameMode>(GameState->AuthorityGameMode) })
	{
		GFCGameMode->OnGameModeCombinedPostLogin.AddUObject(this, &ThisClass::OnPostLogin);
	}
	else if (auto* GFCGameModeBase{ Cast<AGFCGameModeBase>(GameState->AuthorityGameMode) })
	{
		GFCGameModeBase->OnGameModeCombinedPostLogin.AddUObject(this, &ThisClass::OnPostLogin);
	}
	else
	{
		UE_LOG(LogGTE, Warning, TEXT("Can't bind OnPostLogin Event. GameMode must inherit from AGFCGameMode or AGFCGameModeBase to use event binding."));
	}
}

void UTeamManagerComponent::SetTeamCreationData(const UTeamCreationData* NewTeamCreationData)
{
	if (GetOwner()->HasAuthority())
	{
		if (TeamCreationData != NewTeamCreationData)
		{
			TeamCreationData = NewTeamCreationData;

			CheckDefaultInitialization();
		}
	}
}


void UTeamManagerComponent::OnPostLogin(AGameModeBase* GameMode, AController* NewPlayer)
{
	check(NewPlayer);

	auto PlayerState{ NewPlayer->PlayerState };
	check(PlayerState);

	ServerChooseTeamForPlayer(PlayerState);
}

void UTeamManagerComponent::ServerChooseTeamForPlayer(APlayerState* PS)
{
	if (PS->IsOnlyASpectator())
	{
		auto* TMC{ UTeamFunctionLibrary::GetTeamMemberComponentFromActor(PS) };

		TMC->SetGenericTeamId(FGenericTeamId::NoTeam);
	}
	else
	{
		auto TeamAssign{ TeamCreationData->TeamAssignType };
		check(TeamAssign);

		auto* GS{ GetGameStateChecked<AGameStateBase>() };

		TeamAssign->AssignTeamForPlayer(TeamCreationData, PS, GS);
	}
}

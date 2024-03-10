// Copyright (C) 2024 owoDra

#pragma once

#include "Components/GameStateComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"

#include "TeamManagerComponent.generated.h"

class UTeamCreationData;


UCLASS(meta = (BlueprintSpawnableComponent))
class GTEXT_API UTeamManagerComponent 
	: public UGameStateComponent
	, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UTeamManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//
	// Function name used to add this component
	//
	static const FName NAME_ActorFeatureName;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;


protected:
	//
	// Current Team Creation Data
	//
	UPROPERTY(EditAnywhere, ReplicatedUsing = "OnRep_TeamCreationData")
	TObjectPtr<const UTeamCreationData> TeamCreationData{ nullptr };

protected:
	UFUNCTION()
	virtual void OnRep_TeamCreationData();

	/**
	 * Apply the current team creation data
	 */
	virtual void ApplyTeamCreationData();

	/**
	 * Create teams based on Team Creation Data
	 * 
	 * Note:
	 *	Need Authority and TeamCreationData must be valid
	 */
	virtual void ServerCreateTeams();

	/**
	 * Assign players based on Team Creation Data
	 *
	 * Note:
	 *	Need Authority and TeamCreationData must be valid
	 */
	virtual void ServerAssignPlayersToTeams();

public:
	/**
	 * Set the current team creation data
	 */
	UFUNCTION(BlueprintAuthorityOnly, BlueprintCallable)
	void SetTeamCreationData(const UTeamCreationData* NewTeamCreationData);

	/**
	 * Get the current team creation data
	 */
	UFUNCTION(BlueprintCallable)
	const UTeamCreationData* GetTeamCreationData() const { return TeamCreationData; }


protected:
	void OnPostLogin(AGameModeBase* GameMode, AController* NewPlayer);

	/**
	 * Sets the team ID for the given player state.
	 * Spectator-only player states will be stripped of any team association.
	 */
	virtual void ServerChooseTeamForPlayer(APlayerState* PS);

};

﻿// Copyright (C) 2024 owoDra

#pragma once

#include "GameFramework/Info.h"

#include "GameplayTag/GameplayTagStack.h"
#include "GameplayTag/GameplayTagStackInterface.h"

#include "TeamInfoBase.generated.h"

class UTeamManagerSubsystem;


/**
 * Base class for storing team information
 */
UCLASS(Abstract)
class GTEXT_API ATeamInfoBase
	: public AInfo
	, public IGameplayTagStackInterface
{
	GENERATED_BODY()

	friend class UTeamManagerSubsystem;

public:
	ATeamInfoBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	/**
	 * Try to register this TeamInfo in the TeamManagerSubsystem
	 */
	void TryRegisterWithTeamManagerSubsystem();

	/**
	 * Register this TeamInfo in the TeamManagerSubsystem
	 */
	virtual void RegisterWithTeamManagerSubsystem(UTeamManagerSubsystem* Subsystem);
	

protected:
	UPROPERTY(Replicated)
	FGameplayTagStackContainer TeamTags;

protected:
	virtual FGameplayTagStackContainer* GetStatTags() override { return &TeamTags; }
	virtual const FGameplayTagStackContainer* GetStatTagsConst() const override { return &TeamTags; }


protected:
	UPROPERTY(ReplicatedUsing = OnRep_TeamId)
	int32 TeamId{ INDEX_NONE };

protected:
	UFUNCTION()
	void OnRep_TeamId();

public:
	/**
	 * Returns the ID of the team to which this TeamInfo belongs.
	 */
	int32 GetTeamId() const { return TeamId; }

	/**
	 * Set the ID of the team to which this TeamInfo belongs.
	 */
	void SetTeamId(int32 NewTeamId);

	////////////////////////////////////////////////////
	// Game Mode Option
public:
	virtual bool InitializeFromGameModeOption();
	virtual FString ConstructGameModeOption() const;
};

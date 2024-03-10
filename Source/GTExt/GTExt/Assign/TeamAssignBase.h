// Copyright (C) 2024 owoDra

#pragma once

#include "TeamAssignBase.generated.h"

class UTeamCreationData;
class APlayerState;
class AGameState;


/**
 * Base class that defines and executes the method of automatic team assignment
 */
UCLASS(BlueprintType, Const, DefaultToInstanced, EditInlineNew)
class GTEXT_API UTeamAssignBase : public UObject
{
	GENERATED_BODY()
public:
	UTeamAssignBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	/**
	 * Base class that defines and executes the method of automatic team assignment
	 */
	void AssignTeamForPlayer(const UTeamCreationData* TeamCreationData, APlayerState* PlayerState, AGameStateBase* GameState) const;

protected:
	virtual void ProcessAssign(const UTeamCreationData* TeamCreationData, APlayerState* PlayerState, AGameStateBase* GameState) const;
	virtual bool ProcessAssignFromGameModeOption(const UTeamCreationData* TeamCreationData, APlayerState* PlayerState, AGameStateBase* GameState) const;


public:
	virtual FString ConstructGameModeOption(const TArray<APlayerState*>& Players) const;

};

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
	virtual void AssignTeamForPlayer(const UTeamCreationData* TeamCreationData, APlayerState* PlayerState, AGameStateBase* GameState) const;

};

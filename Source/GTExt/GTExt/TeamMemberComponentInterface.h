// Copyright (C) 2024 owoDra

#pragma once

#include "UObject/Interface.h"

#include "TeamMemberComponentInterface.generated.h"

class UTeamMemberComponent;


/**
 * Interface for actors that expose access to an team member component
 */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTeamMemberComponentInterface : public UInterface
{
	GENERATED_BODY()
public:
	UTeamMemberComponentInterface(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};

class GTEXT_API ITeamMemberComponentInterface
{
	GENERATED_BODY()
public:
	ITeamMemberComponentInterface() {}

public:
	/**
	 * Returns the team member component to use for this actor. It may live on another actor, such as a Pawn using the PlayerState's component
	 */
	virtual UTeamMemberComponent* GetTeamMemberComponent() const = 0;

};

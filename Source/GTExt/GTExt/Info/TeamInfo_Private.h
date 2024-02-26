// Copyright (C) 2024 owoDra

#pragma once

#include "TeamInfoBase.h"

#include "TeamInfo_Private.generated.h"


/**
 * Class that stores private information about the team
 *
 * Tips:
 *	Information stored in this class cannot be seen by other teams
 */
UCLASS()
class GTEXT_API ATeamInfo_Private : public ATeamInfoBase
{
	GENERATED_BODY()
public:
	ATeamInfo_Private(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};

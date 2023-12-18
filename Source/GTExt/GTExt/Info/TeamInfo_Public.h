// Copyright (C) 2023 owoDra

#pragma once

#include "TeamInfoBase.h"

#include "TeamInfo_Public.generated.h"

class UTeamDisplayData;


/**
 * Class that stores public information about the team
 * 
 * Tips:
 *	Information stored in this class can be seen by other teams
 */
UCLASS()
class ATeamInfo_Public : public ATeamInfoBase
{
	GENERATED_BODY()

	friend class UTeamManagerSubsystem;

public:
	ATeamInfo_Public(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_TeamDisplayData)
	TObjectPtr<UTeamDisplayData> TeamDisplayData;

protected:
	UFUNCTION()
	void OnRep_TeamDisplayData();

public:
	void SetTeamDisplayData(TObjectPtr<UTeamDisplayData> NewDisplayData);

	UTeamDisplayData* GetTeamDisplayData() const { return TeamDisplayData; }

};

// Copyright (C) 2024 owoDra

#pragma once

#include "TeamTrackingInfo.generated.h"

class ATeamInfoBase;
class ATeamInfo_Public;
class ATeamInfo_Private;
class UTeamDisplayData;


/**
 * Delegate notified that the team's display data has changed
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamDisplayDataChangedDelegate, const UTeamDisplayData*, DisplayData);


/**
 * Data to track current team information
 */
USTRUCT()
struct FTeamTrackingInfo
{
	GENERATED_BODY()
public:
	FTeamTrackingInfo() {}

public:
	UPROPERTY()
	TObjectPtr<ATeamInfo_Public> PublicInfo{ nullptr };

	UPROPERTY()
	TObjectPtr<ATeamInfo_Private> PrivateInfo{ nullptr };

	UPROPERTY()
	TObjectPtr<UTeamDisplayData> DisplayData{ nullptr };

	UPROPERTY()
	FTeamDisplayDataChangedDelegate OnTeamDisplayDataChanged;

public:
	void SetTeamInfo(ATeamInfoBase* Info);
	void RemoveTeamInfo(ATeamInfoBase* Info);

};

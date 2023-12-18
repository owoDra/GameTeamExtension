// Copyright (C) 2023 owoDra

#include "TeamTrackingInfo.h"

#include "Info/TeamInfo_Public.h"
#include "Info/TeamInfo_Private.h"
#include "GTExtLogs.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamTrackingInfo)


void FTeamTrackingInfo::SetTeamInfo(ATeamInfoBase* Info)
{
	// If it is Public Info

	if (auto* NewPublicInfo{ Cast<ATeamInfo_Public>(Info) })
	{
		ensure((PublicInfo == nullptr) || (PublicInfo == NewPublicInfo));

		PublicInfo = NewPublicInfo;

		auto OldDisplayData{ DisplayData };
		DisplayData = NewPublicInfo->GetTeamDisplayData();

		if (OldDisplayData != DisplayData)
		{
			OnTeamDisplayDataChanged.Broadcast(DisplayData);
		}
	}

	// If it is Private Info

	else if (auto* NewPrivateInfo{ Cast<ATeamInfo_Private>(Info) })
	{
		ensure((PrivateInfo == nullptr) || (PrivateInfo == NewPrivateInfo));

		PrivateInfo = NewPrivateInfo;
	}

	// If the Info is invalid

	else
	{
		UE_LOG(LogGTE, Error, TEXT("Expected a public or private team info but got %s"), *GetPathNameSafe(Info));
	}
}

void FTeamTrackingInfo::RemoveTeamInfo(ATeamInfoBase* Info)
{
	// If it is Public Info

	if (PublicInfo == Info)
	{
		PublicInfo = nullptr;
	}

	// If it is Private Info

	else if (PrivateInfo == Info)
	{
		PrivateInfo = nullptr;
	}

	// If the Info is invalid

	else
	{
		UE_LOG(LogGTE, Error, TEXT("Expected a previously registered team info but got %s"), *GetPathNameSafe(Info));
	}
}

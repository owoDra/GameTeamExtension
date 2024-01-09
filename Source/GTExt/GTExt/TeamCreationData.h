// Copyright (C) 2024 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "TeamCreationData.generated.h"

class UTeamDisplayData;
class UTeamAssignBase;
class ATeamInfo_Public;
class ATeamInfo_Private;


/**
 * Data for creating teams in game
 */
UCLASS(BlueprintType, Const)
class UTeamCreationData : public UDataAsset
{
	GENERATED_BODY()
public:
	UTeamCreationData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Teams")
	TMap<uint8, TObjectPtr<UTeamDisplayData>> TeamsToCreate;

	UPROPERTY(EditDefaultsOnly, Category = "Teams")
	TSubclassOf<ATeamInfo_Public> PublicTeamInfoClass;

	UPROPERTY(EditDefaultsOnly, Category = "Teams")
	TSubclassOf<ATeamInfo_Private> PrivateTeamInfoClass;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Teams")
	TObjectPtr<UTeamAssignBase> TeamAssignType;

};

// Copyright (C) 2024 owoDra

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "TeamFunctionLibrary.generated.h"

class UTeamMemberComponent;
struct FGenericTeamId;


UCLASS(MinimalAPI)
class UTeamFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static GTEXT_API int32 GenericTeamIdToInteger(FGenericTeamId ID);

	static GTEXT_API FGenericTeamId IntegerToGenericTeamId(int32 ID);

	/**
	 * Get TeamMemberComponent from actor
	 */
	UFUNCTION(BlueprintPure, Category = "Teams", meta = (BlueprintInternalUseOnly = "false"))
	static GTEXT_API UTeamMemberComponent* GetTeamMemberComponentFromActor(const AActor* Actor, bool LookForComponent = true);

};

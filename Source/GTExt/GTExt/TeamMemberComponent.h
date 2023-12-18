// Copyright (C) 2023 owoDra

#pragma once

#include "Components/ActorComponent.h"
#include "GenericTeamAgentInterface.h"

#include "TeamMemberComponent.generated.h"


/**
 * Delegate to be notified that the team ID you belong to has changed
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTeamIdChangedDelegate, UObject*, ObjectChangingTeam, int32, OldTeamID, int32, NewTeamID);


/**
 * Components for managing teams to which actors belong
 */
UCLASS()
class GTEXT_API UTeamMemberComponent
	: public UActorComponent
	, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	UTeamMemberComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:
	UPROPERTY(BlueprintAssignable)
	FTeamIdChangedDelegate OnTeamChangedDelegate;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

protected:
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return MyTeamID; }


public:
	UFUNCTION(BlueprintPure, Category = "Component")
	static UTeamMemberComponent* FindTeamMemberComponent(const AActor* Actor);

};

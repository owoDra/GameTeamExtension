// Copyright (C) 2024 owoDra

#pragma once

#include "Component/GFCActorComponent.h"
#include "GenericTeamAgentInterface.h"

#include "TeamMemberComponent.generated.h"


/**
 * Delegate to be notified that the team ID you belong to has changed
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTeamIdChangedDelegate, UObject*, ObjectChangingTeam, int32, OldTeamID, int32, NewTeamID);


/**
 * Components for managing teams to which actors belong
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class GTEXT_API UTeamMemberComponent 
	: public UGFCActorComponent
	, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	UTeamMemberComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//
	// Function name used to add this component
	//
	static const FName NAME_ActorFeatureName;

public:
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }


public:
	UPROPERTY(BlueprintAssignable)
	FTeamIdChangedDelegate OnTeamChanged;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_MyTeamID)
	FGenericTeamId MyTeamID;

protected:
	UFUNCTION()
	void OnRep_MyTeamID(FGenericTeamId OldTeamID);

public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override { return MyTeamID; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team")
	int32 GetTeamId() const { return static_cast<int32>(MyTeamID.GetId()); }

public:
	UFUNCTION(BlueprintPure, Category = "Component")
	static UTeamMemberComponent* FindTeamMemberComponent(const AActor* Actor);

};

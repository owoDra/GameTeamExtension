// Copyright (C) 2023 owoDra

#pragma once

#include "Subsystems/WorldSubsystem.h"

#include "TeamTrackingInfo.h"

#include "GameplayTagContainer.h"

#include "TeamManagerSubsystem.generated.h"

class APlayerState;


/**
 * Result of comparing the team affiliation for two actors
 */
UENUM(BlueprintType)
enum class ETeamComparison : uint8
{
	OnSameTeam,			// Both actors are members of the same team

	DifferentTeams,		// The actors are members of opposing teams

	InvalidArgument		// One (or both) of the actors was invalid or not part of any team
};


/** 
 * A subsystem for easy access to team information for team-based actors (e.g., pawns or player states) 
 */
UCLASS()
class GTEXT_API UTeamManagerSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	UTeamManagerSubsystem() {}

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;


protected:
	UPROPERTY()
	TMap<int32, FTeamTrackingInfo> TeamMap;

public:
	void RegisterTeamInfo(ATeamInfoBase* TeamInfo);
	void UnregisterTeamInfo(ATeamInfoBase* TeamInfo);

	/**
	 * Called when a team display data has been edited, causes all team color observers to update
	 */
	void NotifyTeamDisplayDataModified(UTeamDisplayData* ModifiedData);


public:
	/**
	 * Changes the team associated with this actor if possible
	 * 
	 * Note:
	 *	This function can only be called on the authority
	 */
	UFUNCTION(BlueprintCallable, Category = "Teams")
	bool ChangeTeamForActor(AActor* ActorToChange, int32 NewTeamId);

	/**
	 * Returns true if the instigator can damage the target, taking into account the friendly fire settings
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams")
	bool CanCauseDamage(const AActor* Instigator, const AActor* Target, bool bAllowDamageToSelf = true) const;
	
	/**
	 * Compare the teams of two actors and returns a value indicating if they are on same teams, different teams, or one/both are invalid
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams", meta = (ExpandEnumAsExecs = ReturnValue))
	ETeamComparison CompareTeams(const AActor* A, const AActor* B, int32& TeamIdA, int32& TeamIdB) const;
	ETeamComparison CompareTeams(const AActor* A, const AActor* B) const;
	
	/**
	 * Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Teams")
	void AddTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount);

	/**
	 * Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Teams")
	void RemoveTeamTagStack(int32 TeamId, FGameplayTag Tag, int32 StackCount);

	/**
	 * Returns the stack count of the specified tag (or 0 if the tag is not present)
	 */
	UFUNCTION(BlueprintCallable, Category = "Teams")
	int32 GetTeamTagStackCount(int32 TeamId, FGameplayTag Tag) const;

	/**
	 * Returns true if there is at least one stack of the specified tag
	 */
	UFUNCTION(BlueprintCallable, Category = "Teams")
	bool TeamHasTag(int32 TeamId, FGameplayTag Tag) const;

	/**
	 * Returns true if the specified team exists
	 */
	UFUNCTION(BlueprintCallable, Category = "Teams")
	bool DoesTeamExist(int32 TeamId) const;


public:
	/**
	 * Register for a team display data notification for the specified team ID
	 */
	FTeamDisplayDataChangedDelegate& GetTeamDisplayDataChangedDelegate(int32 TeamId);

	/**
	 * Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	 */
	int32 FindTeamFromActor(const AActor* TestActor) const;

	/**
	 * Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams", meta = (Keywords = "Get", DisplayName = "FindTeamFromActor"))
	void BP_FindTeamFromActor(const AActor* TestActor, bool& bIsPartOfTeam, int32& TeamId) const;

	/**
	 * Gets the team display data for the specified team, from the perspective of the specified team
	 * 
	 * Note:
	 *	You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation
	 */
	UFUNCTION(BlueprintCallable, Category = "Teams")
	UTeamDisplayData* GetTeamDisplayData(int32 TeamId, int32 ViewerTeamId);

	/**
	 * Gets the team display data for the specified team, from the perspective of the specified team
	 *
	 * Note:
	 *	You have to specify a viewer too, in case the game mode is in a 'local player is always blue team' sort of situation
	 */
	UFUNCTION(BlueprintCallable, Category = "Teams")
	UTeamDisplayData* GetEffectiveTeamDisplayData(int32 TeamId, AActor* ViewerTeamAgent);

	/**
	 * Gets the list of teams
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams")
	TArray<int32> GetTeamIDs() const;

	/**
	 * Gets the list of enemy teams
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Teams")
	TArray<int32> GetEnemyTeamIDsFromActor(const AActor* TestActor) const;

};

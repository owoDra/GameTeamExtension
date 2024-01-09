// Copyright (C) 2024 owoDra

#include "TeamMemberComponent.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamMemberComponent)


const FName UTeamMemberComponent::NAME_ActorFeatureName("TeamMember");

UTeamMemberComponent::UTeamMemberComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UTeamMemberComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, MyTeamID);
}


void UTeamMemberComponent::OnRep_MyTeamID(FGenericTeamId OldTeamID)
{
}

void UTeamMemberComponent::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (GetOwner()->HasAuthority())
	{
		MyTeamID = NewTeamID;
	}
}


UTeamMemberComponent* UTeamMemberComponent::FindTeamMemberComponent(const AActor* Actor)
{
	return (Actor ? Actor->FindComponentByClass<UTeamMemberComponent>() : nullptr);
}

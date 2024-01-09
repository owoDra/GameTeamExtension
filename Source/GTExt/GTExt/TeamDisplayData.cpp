// Copyright (C) 2024 owoDra

#include "TeamDisplayData.h"

#include "Components/MeshComponent.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/Texture.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TeamDisplayData)


UTeamDisplayData::UTeamDisplayData(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UTeamDisplayData::ApplyToMaterial(UMaterialInstanceDynamic* Material) const
{
	if (Material)
	{
		for (const auto& KVP : ScalarParameters)
		{
			Material->SetScalarParameterValue(KVP.Key, KVP.Value);
		}

		for (const auto& KVP : ColorParameters)
		{
			Material->SetVectorParameterValue(KVP.Key, FVector(KVP.Value));
		}

		for (const auto& KVP : TextureParameters)
		{
			Material->SetTextureParameterValue(KVP.Key, KVP.Value);
		}
	}
}

void UTeamDisplayData::ApplyToMeshComponent(UMeshComponent* MeshComponent) const
{
	if (MeshComponent)
	{
		for (const auto& KVP : ScalarParameters)
		{
			MeshComponent->SetScalarParameterValueOnMaterials(KVP.Key, KVP.Value);
		}

		for (const auto& KVP : ColorParameters)
		{
			MeshComponent->SetVectorParameterValueOnMaterials(KVP.Key, FVector(KVP.Value));
		}

		const auto MaterialInterfaces{ MeshComponent->GetMaterials() };
		for (auto MaterialIndex{ 0 }; MaterialIndex < MaterialInterfaces.Num(); ++MaterialIndex)
		{
			if (const auto& MaterialInterface{ MaterialInterfaces[MaterialIndex] })
			{
				auto* DynamicMaterial{ Cast<UMaterialInstanceDynamic>(MaterialInterface) };

				if (!DynamicMaterial)
				{
					DynamicMaterial = MeshComponent->CreateAndSetMaterialInstanceDynamic(MaterialIndex);
				}

				for (const auto& KVP : TextureParameters)
				{
					DynamicMaterial->SetTextureParameterValue(KVP.Key, KVP.Value);
				}
			}
		}
	}
}

void UTeamDisplayData::ApplyToNiagaraComponent(UNiagaraComponent* NiagaraComponent) const
{
	if (NiagaraComponent)
	{
		for (const auto& KVP : ScalarParameters)
		{
			NiagaraComponent->SetVariableFloat(KVP.Key, KVP.Value);
		}

		for (const auto& KVP : ColorParameters)
		{
			NiagaraComponent->SetVariableLinearColor(KVP.Key, KVP.Value);
		}

		for (const auto& KVP : TextureParameters)
		{
			NiagaraComponent->SetVariableTexture(KVP.Key, KVP.Value);
		}
	}
}

void UTeamDisplayData::ApplyToActor(AActor* TargetActor, bool bIncludeChildActors) const
{
	if (TargetActor != nullptr)
	{
		TargetActor->ForEachComponent(bIncludeChildActors, [this](UActorComponent* InComponent)
		{
			if (auto* MeshComponent{ Cast<UMeshComponent>(InComponent) })
			{
				ApplyToMeshComponent(MeshComponent);
			}
			else if (auto* NiagaraComponent{ Cast<UNiagaraComponent>(InComponent) })
			{
				ApplyToNiagaraComponent(NiagaraComponent);
			}
		});
	}
}

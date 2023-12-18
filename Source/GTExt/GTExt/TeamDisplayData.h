// Copyright (C) 2023 owoDra

#pragma once

#include "Engine/DataAsset.h"

#include "TeamDisplayData.generated.h"

class UMaterialInstanceDynamic;
class UMeshComponent;
class UNiagaraComponent;
class AActor;
class UTexture;


/**
 * Represents the display information for team definitions
 * 
 * For example:
 *	colors, display names, textures, etc...
 */
UCLASS(BlueprintType, Const)
class UTeamDisplayData : public UDataAsset
{
	GENERATED_BODY()
public:
	UTeamDisplayData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FName, float> ScalarParameters;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FName, FLinearColor> ColorParameters;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FName, TObjectPtr<UTexture>> TextureParameters;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FText TeamName;

public:
	UFUNCTION(BlueprintCallable, Category= "Team")
	void ApplyToMaterial(UMaterialInstanceDynamic* Material) const;

	UFUNCTION(BlueprintCallable, Category= "Team")
	void ApplyToMeshComponent(UMeshComponent* MeshComponent) const;

	UFUNCTION(BlueprintCallable, Category= "Team")
	void ApplyToNiagaraComponent(UNiagaraComponent* NiagaraComponent) const;

	UFUNCTION(BlueprintCallable, Category= "Team", meta = (DefaultToSelf = "TargetActor"))
	void ApplyToActor(AActor* TargetActor, bool bIncludeChildActors = true) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Team")
	const FText& GetTeamName() const { return TeamName; }

};

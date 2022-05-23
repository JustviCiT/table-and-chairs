#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralBoxComponent.generated.h"


/*
Helper base class used to procedurally construct all 6 faces of any object
*/
UCLASS(Blueprintable)
class TABLEANDCHAIRS_API UProceduralBoxComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

private:
	void Build(const FVector Size, const TArray<FLinearColor> LinearColors, const TArray<FProcMeshTangent> MeshTangents, bool CollisionEnabled = false);

public:
	UProceduralBoxComponent(const FObjectInitializer& ObjectInitializer);

	/*
	Given a @param Size constructs all 6 faces of a geometry
	*/
	UFUNCTION(BlueprintCallable, Category="TaC")
	void Build(const FVector Size, bool CollisionEnabled = true);

	/**
	Sets the specified material to all box faces
	*/
	UFUNCTION(BlueprintCallable, Category = "TaC")
	void SetBoxMaterial(const UMaterialInterface* const NewMaterial);
};

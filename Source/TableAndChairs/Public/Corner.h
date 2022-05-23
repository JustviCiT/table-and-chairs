#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralBoxComponent.h"
#include "LogTaC.h"
#include "Corner.generated.h"


/*
Class that builds 4 corners
*/
UCLASS(Blueprintable)
class TABLEANDCHAIRS_API ACornerActor : public AActor
{
	GENERATED_BODY()


public:
	ACornerActor();

	static const float ANCHOR_SIZE;
	static const float ANCHOR_HOVER_DISTANCE;

private:

	UPROPERTY()
	FVector OppositeCornerLocation = FVector();

	UPROPERTY()
	FVector SelectedCornerSign = FVector();

	/**
	The four table corners in clockwise order from the positive coordinates relative sector
	*/
	UPROPERTY()
	TArray<UProceduralBoxComponent*> Corners;


protected:

	//virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

public:

	/*
	Returns the cached version of SelectedCornerSign
	*/
	UFUNCTION()
	FVector GetSelectedCornerSign();

	/**
	Returns the opposite corner given a corner

	@Param CurrentCorner the input corner
	@Return opposite corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	UProceduralMeshComponent * GetOppositeCorner(const UProceduralMeshComponent* CurrentCorner) const;
	/**
	Returns the opposite corner location

	@Return OppositeCornerLocation 
	*/
	UFUNCTION()
	FVector GetOppositeCornerLocation();

	/**
	Returns the four anchor corners of the table
	In clockwise order from the positive sector

	X+ Y+, X- Y+, X- Y-, X+ Y-
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	TArray<UProceduralBoxComponent*> GetCorners() const;


	/**
	Returns one corner given one

	@param Index
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	UProceduralBoxComponent* GetCorner(int Index) const;

	/**
	* Changes the texture of a single corner to selected
	* @Param CurrentCorner input corner
	*/
	UFUNCTION(BlueprintCallable, Category = "TaC")
	void SetCornerSelected(const UProceduralMeshComponent* CurrentCorner);

	/**
	* Changes the texture of a single corner to enabled
	* @Param CurrentCorner input corner
	*/
	UFUNCTION(BlueprintCallable, Category = "TaC")
	void SetCornerEnabled(const UProceduralMeshComponent* CurrentCorner);

	/**
	Returns the corner that will have a fixed X

	@Param CurrentCorner the input corner
	@Return X corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	UProceduralMeshComponent* GetFixedXCorner(const UProceduralMeshComponent* CurrentCorner) const;

	/**
	Returns the corner that will have a fixed Y

	@Param CurrentCorner the input corner
	@Return Y corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	UProceduralMeshComponent* GetFixedYCorner(const UProceduralMeshComponent* CurrentCorner) const;

	/**
	The material applied to the corner when it's not enabled, set in the constructor, read only
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TaC")
	UMaterial* CornerMaterialEnabled;

	/**
	The material applied to the corner when it's selected, set in the constructor, read only
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TaC")
	UMaterial* CornerMaterialSelected;
};

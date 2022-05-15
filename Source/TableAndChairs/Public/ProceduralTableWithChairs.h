#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "LogTaC.h"

#include "ProceduralChairWithBackrest.h"
#include "ProceduralTable.h"
#include "ProceduralBoxComponent.h"

#include "ProceduralTableWithChairs.generated.h"

UCLASS(Blueprintable)
class TABLEANDCHAIRS_API ATableActor : public AActor
{
	GENERATED_BODY()


public:
	ATableActor();

	static const float ANCHOR_SIZE;
	static const float ANCHOR_HOVER_DISTANCE;

	static const float DISTANCE_BETWEEN_CHAIRS;

	static const float CHAIRS_DISTANCE_FROM_TABLE;

	static const float CHAIRS_INTERVAL;

	/**
	Table min size is one chair with some distance
	*/
	static const float TABLE_MIN_SIZE;

	static const float TABLE_MAX_SIZE;

	static const int MAX_CHAIRS;

private:

	/**
	The four table corners in clockwise order from the positive coordinates relative sector
	*/
	UPROPERTY()
	TArray<UProceduralBoxComponent*> Corners;

	UPROPERTY()
	AProceduralTable* Table;

protected:

	//virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable, Category = "TaC")
	bool SetCornerWorldLocation(UProceduralMeshComponent * Corner, const FVector NewWorldLocation);

	/**
	Returns the opposite corner given a corner

	@Param CurrentCorner the input corner
	@Return opposite corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	UProceduralMeshComponent * GetOppositeCorner(const UProceduralMeshComponent* CurrentCorner) const;

	/**
	Returns the four anchor corners of the table
	In clockwise order from the positive sector

	X+ Y+, X- Y+, X- Y-, X+ Y-
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	TArray<UProceduralBoxComponent*> GetCorners() const;


	/**
	* Changes the texture of a single corner from selected to enabled
	* @Param CurrentCorner input corner
	*/
	UFUNCTION(BlueprintCallable, Category = "TaC")
	void SetCornerSelected(const UProceduralMeshComponent* CurrentCorner);


	UFUNCTION(BlueprintCallable, Category = "TaC")
	void SetCornerEnabled(const UProceduralMeshComponent* CurrentCorner);

	/**
	Returns the clockwise corner given a corner

	@Param CurrentCorner the input corner
	@Return clockwise corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	UProceduralMeshComponent* GetClockwiseCorner(const UProceduralMeshComponent* const CurrentCorner) const;

	/**
	Returns the counterclockwise corner given a corner

	@Param CurrentCorner the input corner
	@Return counterclockwise corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	UProceduralMeshComponent* GetCounterClockwiseCorner(const UProceduralMeshComponent* const CurrentCorner) const;

	/**
	Call this after editing one of the 4 corners locations to refresh vertexes locations
	*/
	UFUNCTION(BlueprintCallable, Category = "TaC")
	void RefreshLocations();

	/**
	The material applied to the corner when it's not selected, set in the constructor, read only
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TaC")
	UMaterial* CornerMaterialEnabled;

	/**
	The material applied to the corner when it's selected, set in the constructor, read only
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TaC")
	UMaterial* CornerMaterialSelected;
};

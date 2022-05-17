

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralBoxComponent.h"
#include "LogTaC.h"
#include "ProceduralTable.generated.h"

UCLASS()
class TABLEANDCHAIRS_API AProceduralTable : public AActor
{
	GENERATED_BODY()

private:
	
	/**
	The procedural mesh of the table countertop
	*/
	UPROPERTY()
	UProceduralBoxComponent* CounterTop;

	/**
	The four procedural legs
	*/
	UPROPERTY()
	TArray<UProceduralBoxComponent*> Legs;
	
	UPROPERTY()
	TArray<FVector> LegsOffsets;
	
	/**
	Cached TableSize
	The real tablesize is calculated as the distance between the corners
	*/
	FVector2D TableSize = DEFAULT_SIZE;

	UFUNCTION(BlueprintCallable, Category = "TaC")
	void UpdateLegsLocation();

protected:
	virtual void BeginPlay() override;

public:	

	AProceduralTable();

public:	

	static const float LEG_LENGTH;
	static const float LEG_SIDE_SIZE;

	static const bool SHOULD_HAVE_COLLISION;

	static const float TABLE_TOP_THICKNESS;
	/**
	Default table size
	*/
	static const FVector2D DEFAULT_SIZE;

	/**
	The material applied to the entire table, set in the constructor, read only
	*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "TaC")
	UMaterial* TableMaterial;

	/**
	The size of the table
	@Return table size as FVector2D
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	FVector2D GetTableSize() const;


	UFUNCTION(BlueprintPure, Category = "TaC")
	void SetTableSize(const FVector2D& pVector);

	/**
	Returns the total table height: legs + countertop
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	float GetTableHeight() const;

	UFUNCTION(BlueprintPure, Category = "TaC")
	void UpdateTableWorldLocation(FVector& NewWorldLocation);

};

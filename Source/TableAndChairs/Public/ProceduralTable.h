

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralBoxComponent.h"
#include "LogTaC.h"
#include "ProceduralTable.generated.h"


/*
Generates a table procedurally with it's legs
*/
UCLASS(Blueprintable)
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
	
	/*
	Cache of leg positions
	*/
	UPROPERTY()
	TArray<FVector> LegsOffsets;
	
	/**
	Cached TableSize
	The real tablesize is calculated as the distance between the corners
	*/
	FVector2D TableSize = DEFAULT_TABLE_SIZE;

	/*
	Each time table size changes this method is called
	*/
	UFUNCTION(BlueprintCallable, Category = "TaC")
	void UpdateLegsLocation();

protected:
	virtual void BeginPlay() override;

public:	

	AProceduralTable();

	static const float LEG_LENGTH;
	static const float LEG_SIDE_SIZE;
	static const float TABLE_TOP_THICKNESS;

	static const FVector2D DEFAULT_TABLE_SIZE;
	static const float DEFAULT_TABLE_HEIGHT;

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

	/*
	Called for every moving corner 
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	void SetTableSize(const FVector2D& pVector);

	/**
	Returns the total table height: legs + countertop
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	float GetTableHeight() const;

	/*
	For every moving corner this method is called
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	void UpdateTableWorldLocation(FVector& NewWorldLocation);

};

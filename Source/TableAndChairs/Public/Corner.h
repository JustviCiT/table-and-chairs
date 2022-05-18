#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralBoxComponent.h"
#include "LogTaC.h"
#include "Corner.generated.h"

UCLASS(Blueprintable)
class TABLEANDCHAIRS_API ACornerActor : public AActor
{
	GENERATED_BODY()


public:
	ACornerActor();

	static const float ANCHOR_SIZE;
	static const float ANCHOR_HOVER_DISTANCE;

private:

	/**
	The four table corners in clockwise order from the positive coordinates relative sector
	*/
	UPROPERTY()
	TArray<UProceduralBoxComponent*> Corners;

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

	UFUNCTION(BlueprintPure, Category = "TaC")
	int Num() const;

	/**
	Returns the four anchor corners of the table
	In clockwise order from the positive sector

	X+ Y+, X- Y+, X- Y-, X+ Y-
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	TArray<UProceduralBoxComponent*> GetCorners() const;

	UFUNCTION(BlueprintPure, Category = "TaC")
	UProceduralBoxComponent* GetCorner(int Index) const;

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
	UProceduralMeshComponent* GetClockwiseCorner(const UProceduralMeshComponent* CurrentCorner) const;

	/**
	Returns the counterclockwise corner given a corner

	@Param CurrentCorner the input corner
	@Return counterclockwise corner or null if CurrentCorner is not a table corner or is null
	*/
	UFUNCTION(BlueprintPure, Category = "TaC")
	UProceduralMeshComponent* GetCounterClockwiseCorner(const UProceduralMeshComponent* CurrentCorner) const;

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

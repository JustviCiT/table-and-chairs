#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "ProceduralBoxComponent.h"
#include "LogTaC.h"
#include "ProceduralChairWithBackrest.generated.h"


/*
Generates chairs with it's legs and backrest
*/
UCLASS(Blueprintable)
class TABLEANDCHAIRS_API AProceduralChair : public AActor
{
	GENERATED_BODY()

private:
	/**
	The procedural mesh of the chair top
	*/
	UPROPERTY()
	UProceduralBoxComponent* ChairSquare;

	/**
	The procedural mesh of the chair back
	*/
	UPROPERTY()
	UProceduralBoxComponent* ChairBackrest;

	/*
	List of the 4 procedural legs of the chair
	*/
	TArray<UProceduralBoxComponent*> ChairLegs;

public:	
	AProceduralChair();

	static const float CHAIR_SQUARE_SIZE;
	static const float CHAIR_SQUARE_THICKNESS;

	static const float CHAIR_LEG_HEIGHT;
	static const float CHAIR_LEG_SIZE;

	static const float CHAIR_BACKREST_HEIGHT;
	static const float CHAIR_BACKREST_THICKNESS;

	/*
	Material applied to the chair
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TaC")
	UMaterial* Material;
};

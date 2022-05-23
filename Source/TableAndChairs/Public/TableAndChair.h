

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChairPair.h"
#include "ProceduralTable.h"
#include "ProceduralBoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Corner.h"
#include "TableAndChair.generated.h"


/*
Main class that contains a table with it's chairs
*/
UCLASS()
class TABLEANDCHAIRS_API ATableAndChair : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATableAndChair();

	static const float DISTANCE_BETWEEN_CHAIRS;

	static const float CHAIRS_DISTANCE_FROM_TABLE;

	/*
	Sum of DiSTANCE_BETWEEN_CHAIS and its base
	*/
	static const float CHAIRS_INTERVAL;

	/**
	Table min size is one chair with some distance
	*/
	static const float TABLE_MIN_SIZE;

	static const float TABLE_MAX_SIZE;

	static const int MAX_CHAIRS;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*
	Destroy recursively all inner objects
	*/
	virtual void BeginDestroy() override;

private:

	UPROPERTY()
	AProceduralTable* Table;

	UPROPERTY()
	ACornerActor* Corners;

	/*
	Cache of 2 lines of chairs
	*/
	UPROPERTY()
	TArray<FChairCuple>  FrontBackChairs;

	/*
	Cache of 2 lines of chairs
	*/
	UPROPERTY()
	TArray<FChairCuple>  LeftRightChairs;


	/*
	Updates the @param container and spawns @param HowMany chairs
	*/
	UFUNCTION()
	void SpawnChairs(int HowMany, TArray<FChairCuple>& Container);

	/*
	Removes from the @param Container the chairs in excess and returns the number of chairs to spawn
	*/
	UFUNCTION()
	int NumChairsToSpawn(int HowMany, TArray<FChairCuple>& Container);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	ACornerActor* GetCornerActor();

	/*
	Given a @param Corner, update it's location to @param NewWorldLocation
	Updates all other corners location and limits corner movment 
	@return true on success
	*/
	UFUNCTION(BlueprintCallable, Category = "TaC")
	bool SetCornerWorldLocation(UProceduralMeshComponent* Corner, const FVector NewWorldLocation);

	/**
	Call this after editing one of the 4 corners locations to refresh vertexes locations
	*/
	UFUNCTION(BlueprintCallable, Category = "TaC")
	void RefreshLocations();

};

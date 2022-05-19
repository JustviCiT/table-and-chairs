

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChairPair.h"
#include "ProceduralTable.h"
#include "ProceduralBoxComponent.h"
#include "Corner.h"
#include "TableAndChair.generated.h"

UCLASS()
class TABLEANDCHAIRS_API ATableAndChair : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATableAndChair();

	static const float DISTANCE_BETWEEN_CHAIRS;

	static const float CHAIRS_DISTANCE_FROM_TABLE;

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
	virtual void BeginDestroy() override;

private:

	UPROPERTY()
	AProceduralTable* Table;

	UPROPERTY()
	ACornerActor* Corners;

	UPROPERTY()
	TArray<FChairCuple>  FrontBackChairs;

	UPROPERTY()
	TArray<FChairCuple>  LeftRightChairs;

	UFUNCTION()
	void SpawnChairs(int HowMany, TArray<FChairCuple>& Container);

	UFUNCTION()
	int NumChairsToSpawn(int HowMany, TArray<FChairCuple>& Container);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	ACornerActor* GetCornerActor();

	UFUNCTION(BlueprintCallable, Category = "TaC")
	bool SetCornerWorldLocation(UProceduralMeshComponent* Corner, const FVector NewWorldLocation);


	/**
	Call this after editing one of the 4 corners locations to refresh vertexes locations
	*/
	UFUNCTION(BlueprintCallable, Category = "TaC")
	void RefreshLocations();

};

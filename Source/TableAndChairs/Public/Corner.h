

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Corner.generated.h"

UCLASS()
class TABLEANDCHAIRS_API ACorner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACorner();

	int m_totalCorners;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

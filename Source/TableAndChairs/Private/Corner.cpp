#include "Corner.h"

// Sets default values
ACorner::ACorner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACorner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACorner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


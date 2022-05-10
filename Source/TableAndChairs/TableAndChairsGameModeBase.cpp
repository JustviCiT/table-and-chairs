#include "TableAndChairsGameModeBase.h"

ATableAndChairsGameModeBase::ATableAndChairsGameModeBase() 
{
	PlayerControllerClass = ATableAndChairsPlayerController::StaticClass();
}

void ATableAndChairsGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->SpawnActor<ATableActor>(FVector::ZeroVector, FRotator::ZeroRotator);
}
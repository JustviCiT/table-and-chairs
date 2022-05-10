#include "ProceduralChairWithBackrest.h"

AProceduralChair::AProceduralChair()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	//static ConstructorHelpers::FObjectFinder<UMaterial> MaterialChair(TEXT("Material'/Game/Materials/GenericMaterial.GenericMaterial'"));
	//if (MaterialChair.Succeeded())
	//{
	//	Material = MaterialChair.Object;
	//	UE_LOG(LogTaC, Log, TEXT("Chair material loaded"));
	//}
	//else
	//{
	//	UE_LOG(LogTaC, Error, TEXT("Chair material loading error"));
	//}

	// Spawn the chair legs
	TArray<FVector> LegsCorners = {
		{CHAIR_SQUARE_SIZE / 2 - CHAIR_LEG_SIZE / 2,CHAIR_SQUARE_SIZE / 2 - CHAIR_LEG_SIZE / 2,CHAIR_LEG_HEIGHT / 2},
		{CHAIR_SQUARE_SIZE / 2 - CHAIR_LEG_SIZE / 2,-CHAIR_SQUARE_SIZE / 2 + CHAIR_LEG_SIZE / 2,CHAIR_LEG_HEIGHT / 2 },
		{-CHAIR_SQUARE_SIZE / 2 + CHAIR_LEG_SIZE / 2,CHAIR_SQUARE_SIZE / 2 - CHAIR_LEG_SIZE / 2,CHAIR_LEG_HEIGHT / 2 },
		{-CHAIR_SQUARE_SIZE / 2 + CHAIR_LEG_SIZE / 2,-CHAIR_SQUARE_SIZE / 2 + CHAIR_LEG_SIZE / 2,CHAIR_LEG_HEIGHT / 2 }
	};
	for (size_t i = 0; i < LegsCorners.Num(); i++)
	{
		const FString LegName = "Leg" + FString::FromInt(i);
		auto LegComp = CreateDefaultSubobject<UProceduralBoxComponent>(*LegName);
		LegComp->Build(FVector(CHAIR_LEG_SIZE, CHAIR_LEG_SIZE, CHAIR_LEG_HEIGHT), false);
		LegComp->SetupAttachment(RootComponent);
		LegComp->SetRelativeLocation(LegsCorners[i]);
		LegComp->SetBoxMaterial(Material);
	}

	// Spawn the chair square
	auto ChairSquare = CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("ChairSquare"));
	ChairSquare->Build(FVector(CHAIR_SQUARE_SIZE, CHAIR_SQUARE_SIZE, CHAIR_SQUARE_THICKNESS), false);
	ChairSquare->SetupAttachment(RootComponent);
	ChairSquare->SetRelativeLocation(FVector(0,0, CHAIR_LEG_HEIGHT+ CHAIR_SQUARE_THICKNESS/2));
	ChairSquare->SetBoxMaterial(Material);

	// Spawn the chair backrest
	auto ChairBackrest = CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("ChairBackrest"));
	ChairBackrest->Build(FVector(CHAIR_BACKREST_THICKNESS, CHAIR_SQUARE_SIZE, CHAIR_BACKREST_HEIGHT), false);
	ChairBackrest->SetupAttachment(RootComponent);
	ChairBackrest->SetRelativeLocation(FVector(-CHAIR_SQUARE_SIZE/2+CHAIR_BACKREST_THICKNESS/2, 0, CHAIR_LEG_HEIGHT + CHAIR_BACKREST_HEIGHT / 2));
	ChairBackrest->SetBoxMaterial(Material);
}

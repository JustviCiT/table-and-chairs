#include "ProceduralChairWithBackrest.h"

const float AProceduralChair::CHAIR_SQUARE_SIZE = 30;
const float AProceduralChair::CHAIR_SQUARE_THICKNESS = 2;

const float AProceduralChair::CHAIR_LEG_HEIGHT = 45;
const float AProceduralChair::CHAIR_LEG_SIZE = 4;

const float AProceduralChair::CHAIR_BACKREST_HEIGHT = 60;
const float AProceduralChair::CHAIR_BACKREST_THICKNESS = 2;

AProceduralChair::AProceduralChair()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	const float tmpChairSquare = CHAIR_SQUARE_SIZE * 0.5f;
	const float tmpChairLeg = CHAIR_LEG_SIZE * 0.5f;
	const float tmpChairHeight = CHAIR_LEG_HEIGHT * 0.5f;

	// Spawn the chair legs
	TArray<FVector> LegsCorners = {
		{ tmpChairSquare - tmpChairLeg, tmpChairSquare - tmpChairLeg, tmpChairHeight },
		{ tmpChairSquare - tmpChairLeg,-tmpChairSquare + tmpChairLeg, tmpChairHeight },
		{-tmpChairSquare + tmpChairLeg, tmpChairSquare - tmpChairLeg, tmpChairHeight },
		{-tmpChairSquare + tmpChairLeg,-tmpChairSquare + tmpChairLeg, tmpChairHeight }
	};
	for (size_t i = 0; i < LegsCorners.Num(); i++)
	{
		const FString LegName = "Leg" + FString::FromInt(i);
		auto LegComp = CreateDefaultSubobject<UProceduralBoxComponent>(*LegName);
		LegComp->Build(FVector(CHAIR_LEG_SIZE, CHAIR_LEG_SIZE, CHAIR_LEG_HEIGHT));
		LegComp->SetupAttachment(RootComponent);
		LegComp->SetRelativeLocation(LegsCorners[i]);
		LegComp->SetBoxMaterial(Material);
	}

	// Spawn the chair square
	ChairSquare = CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("ChairSquare"));
	ChairSquare->Build(FVector(CHAIR_SQUARE_SIZE, CHAIR_SQUARE_SIZE, CHAIR_SQUARE_THICKNESS));
	ChairSquare->SetupAttachment(RootComponent);
	ChairSquare->SetRelativeLocation(FVector(0,0, CHAIR_LEG_HEIGHT+ CHAIR_SQUARE_THICKNESS/2));
	ChairSquare->SetBoxMaterial(Material);

	// Spawn the chair backrest
	ChairBackrest = CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("ChairBackrest"));
	ChairBackrest->Build(FVector(CHAIR_BACKREST_THICKNESS, CHAIR_SQUARE_SIZE, CHAIR_BACKREST_HEIGHT));
	ChairBackrest->SetupAttachment(RootComponent);
	ChairBackrest->SetRelativeLocation(FVector(-tmpChairSquare + CHAIR_BACKREST_THICKNESS/2, 0, CHAIR_LEG_HEIGHT + CHAIR_BACKREST_HEIGHT * 0.5f));
	ChairBackrest->SetBoxMaterial(Material);
}

#include "Corner.h"

const float		ACornerActor::ANCHOR_SIZE = 25;
const float		ACornerActor::ANCHOR_HOVER_DISTANCE = 10;

ACornerActor::ACornerActor()
{
	// Turn the table tick off, we don't need it
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// Create resizable corners
	Corners = {
		CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("XPositiveYPositive")),
		CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("XNegativeYPositive")),
		CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("XNegativeYNegative")),
		CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("XPositiveYNegative"))
	};

	for (size_t i = 0; i < Corners.Num(); i++)
	{
		Corners[i]->SetupAttachment(RootComponent);
		Corners[i]->Build(FVector(ANCHOR_SIZE, ANCHOR_SIZE, 1), true);
		//Corners[i]->ContainsPhysicsTriMeshData(true);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialTileAnchor(TEXT("Material'/Game/Materials/M_TileAnchor.M_TileAnchor'"));
	if (MaterialTileAnchor.Succeeded())
	{
		CornerMaterialEnabled = MaterialTileAnchor.Object;
		for (size_t i = 0; i < Corners.Num(); i++)
			Corners[i]->SetBoxMaterial(CornerMaterialEnabled);
		UE_LOG(LogTaC, Log, TEXT("Resize anchor material enabled loaded"));
	}
	else
	{
		UE_LOG(LogTaC, Error, TEXT("Resize anchor material enabled failed loading"));
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialTileAnchorSelected(TEXT("Material'/Game/Materials/M_TileAnchorSelected.M_TileAnchorSelected'"));
	if (MaterialTileAnchorSelected.Succeeded())
	{
		CornerMaterialSelected = MaterialTileAnchorSelected.Object;
		UE_LOG(LogTaC, Log, TEXT("Resize anchor material selected loaded"));
	}
	else
	{
		UE_LOG(LogTaC, Error, TEXT("Resize anchor material selected failed loading"));
	}

}

bool ACornerActor::SetCornerWorldLocation(UProceduralMeshComponent * Corner, const FVector NewCornerWorldLocation)
{
	if (Corner == nullptr) 
		return false;

	const FVector OldLocation = Corner->GetComponentLocation();
	//UE_LOG(LogTaC, Log, TEXT("Old Location %s"), *OldLocation.ToString());
	const UProceduralMeshComponent* OppositeCorner = GetOppositeCorner(Corner);
	const FVector OppositeLocation = GetOppositeCorner(Corner)->GetRelativeLocation();
	//const FVector OldSign = (OldLocation - OppositeCorner->GetRelativeTransform().GetLocation()).GetSignVector();
	const FVector OldSign = (OldLocation - OppositeLocation).GetSignVector();
	//UE_LOG(LogTaC, Log, TEXT("Old Sign %s"), *OldSign.ToString());
	// 
	//// The current corner location is where the mouse ray hits the table plane
	Corner->SetWorldLocation(NewCornerWorldLocation);

	//// The nearby corners location depends if they are the clockwise/counterclockwise corners and in which sector of the table they are
	UProceduralMeshComponent* ClockwiseCorner = GetClockwiseCorner(Corner);
	////ensureMsgf(ClockwiseCorner != OppositeCorner, TEXT("A clockwise corner can't be the opposite corner"));
	UProceduralMeshComponent* CounterClockwiseCorner = GetCounterClockwiseCorner(Corner);
	//ensureMsgf(CounterClockwiseCorner != OppositeCorner, TEXT("A counterclockwise corner can't be the opposite corner"));
	const FVector NewSign = (Corner->GetRelativeLocation() - OppositeLocation).GetSignVector();
	UE_LOG(LogTaC, Log, TEXT("New Sign %s"), *NewSign.ToString());

	// HACK: fix this ugly switch
	// If a corner is in a quadrant in which both coordinates have the same sign it should assign the Y on the clockwise corner and the X on the counterclockwise corner
	// If a corner is in a quadrant in which both coordinates have a different sign it should assign the X on the clockwise corner and the Y on the counterclockwise corner
	UProceduralMeshComponent* Corner1;
	UProceduralMeshComponent* Corner2;

	int CornerIndex = Corners.IndexOfByKey(Corner);
	if (CornerIndex == INDEX_NONE)
		return false;

	switch (CornerIndex)
	{
	case 0:
	case 2:
		Corner1 = ClockwiseCorner;
		Corner2 = CounterClockwiseCorner;
		break;
	case 1:
	case 3:
		Corner1 = CounterClockwiseCorner;
		Corner2 = ClockwiseCorner;
		break;
	default:
		checkNoEntry();
		return false;
	}
	const FVector Corner1OldLoc = Corner1->GetComponentLocation();
	const FVector Corner2OldLoc = Corner2->GetComponentLocation();
	Corner1->SetWorldLocation(FVector(Corner1OldLoc.X, NewCornerWorldLocation.Y, Corner1OldLoc.Z));
	Corner2->SetWorldLocation(FVector(NewCornerWorldLocation.X, Corner2OldLoc.Y, Corner2OldLoc.Z));

	const float DistanceClockwise = FVector::Distance(NewCornerWorldLocation, ClockwiseCorner->GetComponentLocation());
	const float DistanceCounterClockwise = FVector::Distance(NewCornerWorldLocation, CounterClockwiseCorner->GetComponentLocation());

	//// Minimum table area check
	//if (DistanceClockwise < ATableAndChair::TABLE_MIN_SIZE ||
	//	DistanceCounterClockwise < ATableAndChair::TABLE_MIN_SIZE ||
	//	DistanceClockwise > ATableAndChair::TABLE_MAX_SIZE ||
	//	DistanceCounterClockwise > ATableAndChair::TABLE_MAX_SIZE ||
	//	OldSign != NewSign
	//	)
	//{
	//	Corner->SetWorldLocation(OldLocation);
	//	Corner1->SetWorldLocation(Corner1OldLoc);
	//	Corner2->SetWorldLocation(Corner2OldLoc);
	//	return false;
	//}

	return true;
}

void ACornerActor::BeginPlay()
{
	Super::BeginPlay();
}

TArray<UProceduralBoxComponent*> ACornerActor::GetCorners() const
{
	return Corners;
}

UProceduralBoxComponent* ACornerActor::GetCorner(int Index) const
{
	if (Index > Corners.Num() || Index < 0)
		return nullptr;

	return Corners[Index];
}

void ACornerActor::SetCornerSelected(const UProceduralMeshComponent* CurrentCorner)
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE) {
		UE_LOG(LogTaC, Error, TEXT("CurrentCorner index is oob"));
		return;
	}

	Corners[index]->SetBoxMaterial(CornerMaterialSelected);
}

void ACornerActor::SetCornerEnabled(const UProceduralMeshComponent* CurrentCorner)
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE) {
		UE_LOG(LogTaC, Error, TEXT("CurrentCorner index is oob"));
		return;
	}

	Corners[index]->SetBoxMaterial(CornerMaterialEnabled);
}


UProceduralMeshComponent * ACornerActor::GetOppositeCorner(const UProceduralMeshComponent * CurrentCorner) const
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE)
		return nullptr;
	return Corners[(index+2) % 4];
}

int ACornerActor::Num() const
{
	return Corners.Num();
}

UProceduralMeshComponent* ACornerActor::GetClockwiseCorner(const UProceduralMeshComponent* CurrentCorner) const
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE)
		return nullptr;

	return Corners[(index+1) % 4];
}

UProceduralMeshComponent* ACornerActor::GetCounterClockwiseCorner(const UProceduralMeshComponent* CurrentCorner) const
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE)
		return nullptr;

	return Corners[(index+3) % 4];
}

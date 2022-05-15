#include "ProceduralTableWithChairs.h"


#ifdef UE_BUILD_DEVELOPMENT
#	include "Engine.h"
#endif


const float		ATableActor::ANCHOR_SIZE = 25;
const float		ATableActor::ANCHOR_HOVER_DISTANCE = 10;
const float		ATableActor::DISTANCE_BETWEEN_CHAIRS = 50;
const float		ATableActor::CHAIRS_DISTANCE_FROM_TABLE = 25;
const float		ATableActor::CHAIRS_INTERVAL = (AProceduralChair::CHAIR_SQUARE_SIZE + DISTANCE_BETWEEN_CHAIRS);
const float		ATableActor::TABLE_MIN_SIZE = (AProceduralChair::CHAIR_SQUARE_SIZE + DISTANCE_BETWEEN_CHAIRS * 2);
const int		ATableActor::MAX_CHAIRS = 100;
const float		ATableActor::TABLE_MAX_SIZE = TABLE_MIN_SIZE * MAX_CHAIRS - DISTANCE_BETWEEN_CHAIRS * (MAX_CHAIRS - 1);

ATableActor::ATableActor()
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

bool ATableActor::SetCornerWorldLocation(UProceduralMeshComponent * Corner, const FVector NewCornerWorldLocation)
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
	//UE_LOG(LogTaC, Log, TEXT("New Sign %s"), *NewSign.ToString());

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
	if (DistanceClockwise < ATableActor::TABLE_MIN_SIZE ||
		DistanceCounterClockwise < ATableActor::TABLE_MIN_SIZE ||
		DistanceClockwise > ATableActor::TABLE_MAX_SIZE ||
		DistanceCounterClockwise > ATableActor::TABLE_MAX_SIZE ||
		OldSign != NewSign
		)
	{
		Corner->SetWorldLocation(OldLocation);
		Corner1->SetWorldLocation(Corner1OldLoc);
		Corner2->SetWorldLocation(Corner2OldLoc);
		return false;
	}

	// Update the table vertexes
	RefreshLocations();

	return true;
}


void ATableActor::BeginPlay()
{
	Super::BeginPlay();

	Table = GetWorld()->SpawnActor<AProceduralTable>(AProceduralTable::StaticClass(), this->GetActorTransform());
	Table->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	Corners[0]->SetRelativeLocation(FVector(Table->GetTableSize().X / 2, Table->GetTableSize().Y / 2, Table->GetTableHeight() + ANCHOR_HOVER_DISTANCE));
	Corners[1]->SetRelativeLocation(FVector(-Table->GetTableSize().X / 2, Table->GetTableSize().Y / 2, Table->GetTableHeight() + ANCHOR_HOVER_DISTANCE));
	Corners[2]->SetRelativeLocation(FVector(-Table->GetTableSize().X / 2, -Table->GetTableSize().Y / 2, Table->GetTableHeight() + ANCHOR_HOVER_DISTANCE));
	Corners[3]->SetRelativeLocation(FVector(Table->GetTableSize().X / 2, -Table->GetTableSize().Y / 2, Table->GetTableHeight() + ANCHOR_HOVER_DISTANCE));

	UE_LOG(LogTaC, Log, TEXT("This is called ..."));
	RefreshLocations();
}

//void ATableActor::OnConstruction(const FTransform & Transform)
//{
//	Super::OnConstruction(Transform);
//
//
//}

TArray<UProceduralBoxComponent*> ATableActor::GetCorners() const
{
	return Corners;
}

void ATableActor::SetCornerSelected(const UProceduralMeshComponent* CurrentCorner)
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE) {
		UE_LOG(LogTaC, Error, TEXT("CurrentCorner index is oob"));
		return;
	}

	Corners[index]->SetBoxMaterial(CornerMaterialSelected);
}

void ATableActor::SetCornerEnabled(const UProceduralMeshComponent* CurrentCorner)
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE) {
		UE_LOG(LogTaC, Error, TEXT("CurrentCorner index is oob"));
		return;
	}

	Corners[index]->SetBoxMaterial(CornerMaterialEnabled);
}


UProceduralMeshComponent * ATableActor::GetOppositeCorner(const UProceduralMeshComponent * CurrentCorner) const
{
	// HACK: fix this with a lookup table maybe
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE)
		return nullptr;
	return Corners[(index+2) % 4];
}

UProceduralMeshComponent* ATableActor::GetClockwiseCorner(const UProceduralMeshComponent* CurrentCorner) const
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE)
		return nullptr;

	return Corners[(index+1) % 4];
}

UProceduralMeshComponent* ATableActor::GetCounterClockwiseCorner(const UProceduralMeshComponent* CurrentCorner) const
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE)
		return nullptr;

	return Corners[(index+3) % 4];
}

void ATableActor::RefreshLocations()
{
	// Cache table size
	FVector2D TmpTableSize = FVector2D(
		FVector::Distance(Corners[0]->GetComponentLocation(), Corners[1]->GetComponentLocation()),
		FVector::Distance(Corners[0]->GetComponentLocation(), Corners[3]->GetComponentLocation())
	);


	// HACK: fix root location
	FVector NewRelativeRoot = FVector(
		Corners[0]->GetRelativeLocation().X - Table->GetTableSize().X / 2,
		Corners[0]->GetRelativeLocation().Y - Table->GetTableSize().Y / 2,
		Table->GetTableHeight()
	);
	FVector NewWorldRoot = this->GetTransform().TransformPosition(NewRelativeRoot);

	Table->UpdateTableWorldLocation(NewWorldRoot);
	Table->SetTableSize(TmpTableSize);

	// Remove old chairs
	// TODO instead of removing all add the missing one 
	TArray<AActor*> SpawnedChairs;
	this->GetAllChildActors(SpawnedChairs);
	for (size_t i = 0; i < SpawnedChairs.Num(); i++)
		SpawnedChairs[i]->Destroy();

	int ChairsToSpawnOnYSide = Table->GetTableSize().Y / CHAIRS_INTERVAL;
	int ChairsToSpawnOnXSide = Table->GetTableSize().X / CHAIRS_INTERVAL;

	// The offset to add so the chairs line is centered
	// Remaining space / 2
	const float CHAIRS_YLINE_LENGTH = ChairsToSpawnOnYSide * AProceduralChair::CHAIR_SQUARE_SIZE + (ChairsToSpawnOnYSide-1)*DISTANCE_BETWEEN_CHAIRS;
	const float YChairsSpawnOffset = (Table->GetTableSize().Y - CHAIRS_YLINE_LENGTH ) / 2;
	

	// Backward side
	FVector ChairsForwardSpawnPoint = Corners[2]->GetComponentLocation();
	ChairsForwardSpawnPoint.X -= CHAIRS_DISTANCE_FROM_TABLE;
	ChairsForwardSpawnPoint.Y += AProceduralChair::CHAIR_SQUARE_SIZE/2;
	ChairsForwardSpawnPoint.Z = this->GetActorLocation().Z;
	
	for (int y = 0; y < ChairsToSpawnOnYSide; y++)
	{	
		UChildActorComponent* NewChair = NewObject<UChildActorComponent>(this);
		NewChair->SetupAttachment(RootComponent);
		NewChair->SetChildActorClass(AProceduralChair::StaticClass());
		NewChair->SetWorldLocation(FVector(ChairsForwardSpawnPoint.X, YChairsSpawnOffset + ChairsForwardSpawnPoint.Y + y * CHAIRS_INTERVAL, this->GetActorLocation().Z));
	}

	// Forward side
	FVector ChairsBackwardSpawnPoint = Corners[3]->GetComponentLocation();
	ChairsBackwardSpawnPoint.X += CHAIRS_DISTANCE_FROM_TABLE;
	ChairsBackwardSpawnPoint.Y += AProceduralChair::CHAIR_SQUARE_SIZE/2;
	ChairsBackwardSpawnPoint.Z = this->GetActorLocation().Z;
	for (int y = 0; y < ChairsToSpawnOnYSide; y++)
	{
		UChildActorComponent* NewChair = NewObject<UChildActorComponent>(this);
		NewChair->SetupAttachment(RootComponent);
		NewChair->SetChildActorClass(AProceduralChair::StaticClass());
		NewChair->SetWorldLocation(FVector(ChairsBackwardSpawnPoint.X, YChairsSpawnOffset + ChairsBackwardSpawnPoint.Y + y * CHAIRS_INTERVAL, this->GetActorLocation().Z));
		NewChair->SetRelativeRotation(FRotator(0, -180, 0));
	}

	const float CHAIRS_XLINE_LENGTH = ChairsToSpawnOnXSide * AProceduralChair::CHAIR_SQUARE_SIZE + (ChairsToSpawnOnXSide - 1)*DISTANCE_BETWEEN_CHAIRS;
	const float XChairsSpawnOffset = (Table->GetTableSize().X - CHAIRS_XLINE_LENGTH) / 2;

	// Left side
	FVector ChairsLeftSpawnPoint = Corners[2]->GetComponentLocation();
	ChairsLeftSpawnPoint.Y -= CHAIRS_DISTANCE_FROM_TABLE;
	ChairsLeftSpawnPoint.X += AProceduralChair::CHAIR_SQUARE_SIZE / 2;
	ChairsBackwardSpawnPoint.Z = this->GetActorLocation().Z;
	for (int x = 0; x < ChairsToSpawnOnXSide; x++)
	{
		UChildActorComponent* NewChair = NewObject<UChildActorComponent>(this);
		NewChair->SetupAttachment(RootComponent);
		NewChair->SetChildActorClass(AProceduralChair::StaticClass());
		NewChair->SetWorldLocation(FVector(XChairsSpawnOffset + ChairsLeftSpawnPoint.X + x * CHAIRS_INTERVAL, ChairsLeftSpawnPoint.Y, this->GetActorLocation().Z));
		NewChair->SetRelativeRotation(FRotator(0, 90, 0));
	}

	// Right side
	FVector ChairsRightSpawnPoint = Corners[1]->GetComponentLocation();
	ChairsRightSpawnPoint.Y += CHAIRS_DISTANCE_FROM_TABLE;
	ChairsRightSpawnPoint.X += AProceduralChair::CHAIR_SQUARE_SIZE / 2;
	ChairsRightSpawnPoint.Z = this->GetActorLocation().Z;
	for (int x = 0; x < ChairsToSpawnOnXSide; x++)
	{
		UChildActorComponent* NewChair = NewObject<UChildActorComponent>(this);
		NewChair->SetupAttachment(RootComponent);
		NewChair->SetChildActorClass(AProceduralChair::StaticClass());
		NewChair->SetWorldLocation(FVector(XChairsSpawnOffset + ChairsRightSpawnPoint.X + x * CHAIRS_INTERVAL, ChairsRightSpawnPoint.Y, this->GetActorLocation().Z));
		NewChair->SetRelativeRotation(FRotator(0, -90, 0));
	}

	// Register the chairs
	RegisterAllComponents();
}

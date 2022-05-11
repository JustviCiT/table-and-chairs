#include "ProceduralTableWithChairs.h"


#ifdef UE_BUILD_DEVELOPMENT
#	include "Engine.h"
#endif

const FVector2D ATableActor::DEFAULT_SIZE = FVector2D(200, 200);

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
		Corners[i]->ContainsPhysicsTriMeshData(true);
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


	Corners[0]->SetRelativeLocation(FVector( TableSize.X / 2,  TableSize.Y / 2, LEG_LENGTH + TABLE_TOP_THICKNESS + ANCHOR_HOVER_DISTANCE));
	Corners[1]->SetRelativeLocation(FVector(-TableSize.X / 2,  TableSize.Y / 2, LEG_LENGTH + TABLE_TOP_THICKNESS + ANCHOR_HOVER_DISTANCE));
	Corners[2]->SetRelativeLocation(FVector(-TableSize.X / 2, -TableSize.Y / 2, LEG_LENGTH + TABLE_TOP_THICKNESS + ANCHOR_HOVER_DISTANCE));
	Corners[3]->SetRelativeLocation(FVector( TableSize.X / 2, -TableSize.Y / 2, LEG_LENGTH + TABLE_TOP_THICKNESS + ANCHOR_HOVER_DISTANCE));


	// Load table material
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/GenericMaterial.GenericMaterial'"));
	if (Material.Succeeded())
	{
		TableMaterial = Material.Object;
		UE_LOG(LogTaC, Log, TEXT("Table material loaded"));
	}
	else
	{
		UE_LOG(LogTaC, Error, TEXT("Table material failed loading"));
	}

	// Create countertop
	CounterTop = CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("Countertop"));
	CounterTop->SetupAttachment(RootComponent);
	CounterTop->Build(FVector(TableSize, TABLE_TOP_THICKNESS), false);
	CounterTop->SetBoxMaterial(TableMaterial);

	// Create legs
	for (size_t i = 0; i < Corners.Num(); i++)
	{
		const FString LegName = "Leg" + FString::FromInt(i);
		auto LegComp = CreateDefaultSubobject<UProceduralBoxComponent>(*LegName);
		LegComp->Build(FVector(LEG_SIDE_SIZE, LEG_SIDE_SIZE, LEG_LENGTH), false);
		LegComp->SetupAttachment(RootComponent);
		LegComp->SetRelativeLocation(FVector::ZeroVector);
		LegComp->SetBoxMaterial(TableMaterial);
		Legs.Add(LegComp);
	}
}

bool ATableActor::SetCornerWorldLocation(UProceduralMeshComponent * Corner, const FVector NewCornerWorldLocation)
{
	if (Corner == nullptr) 
		return false;

	const FVector OldLocation = Corner->GetComponentLocation();
	UE_LOG(LogTaC, Log, TEXT("Old Location %s"), *OldLocation.ToString());
	//const UProceduralMeshComponent* OppositeCorner = GetOppositeCorner(Corner);
	const FVector OppositeLocation = GetOppositeCorner(Corner)->GetComponentLocation();
	//const FVector OldSign = (OldLocation - OppositeCorner->GetRelativeTransform().GetLocation()).GetSignVector();
	const FVector OldSign = (OldLocation - OppositeLocation).GetSignVector();
	UE_LOG(LogTaC, Log, TEXT("Old Sign %s"), *OldSign.ToString());
	// The current corner location is where the mouse ray hits the table plane
	Corner->SetWorldLocation(NewCornerWorldLocation);

	// The nearby corners location depends if they are the clockwise/counterclockwise corners and in which sector of the table they are
	UProceduralMeshComponent* ClockwiseCorner = GetClockwiseCorner(Corner);
	//ensureMsgf(ClockwiseCorner != OppositeCorner, TEXT("A clockwise corner can't be the opposite corner"));
	UProceduralMeshComponent* CounterClockwiseCorner = GetCounterClockwiseCorner(Corner);
	//ensureMsgf(CounterClockwiseCorner != OppositeCorner, TEXT("A counterclockwise corner can't be the opposite corner"));
	const FVector NewSign = (Corner->GetRelativeTransform().GetLocation() - OppositeLocation).GetSignVector();
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
		Corner1 = nullptr;
		Corner2 = nullptr;
	}
	const auto Corner1OldLoc = Corner1->GetComponentLocation();
	const auto Corner2OldLoc = Corner2->GetComponentLocation();
	Corner1->SetWorldLocation(FVector(Corner1->GetComponentLocation().X, NewCornerWorldLocation.Y, Corner1->GetComponentLocation().Z));
	Corner2->SetWorldLocation(FVector(NewCornerWorldLocation.X, Corner2->GetComponentLocation().Y, Corner2->GetComponentLocation().Z));

	// Minimum table area check
	// if corner in other relative sector or distance with relative (0,0) too small abort the resize
	if (FVector::Distance(NewCornerWorldLocation, ClockwiseCorner->GetComponentLocation()) < ATableActor::TABLE_MIN_SIZE ||
		FVector::Distance(NewCornerWorldLocation, CounterClockwiseCorner->GetComponentLocation()) < ATableActor::TABLE_MIN_SIZE ||
		FVector::Distance(NewCornerWorldLocation, ClockwiseCorner->GetComponentLocation()) > ATableActor::TABLE_MAX_SIZE ||
		FVector::Distance(NewCornerWorldLocation, CounterClockwiseCorner->GetComponentLocation()) > ATableActor::TABLE_MAX_SIZE ||
		OldSign != NewSign
		)
	{
		Corner->SetWorldLocation(OldLocation);
		Corner1->SetWorldLocation(Corner1OldLoc);
		Corner2->SetWorldLocation(Corner2OldLoc);
		return false;
	}
	// TODO: support table rotations

	// Update the table vertexes
	RefreshLocations();

	return true;
}


FVector2D ATableActor::GetTableSize() const
{
	return TableSize;
}


void ATableActor::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);
	RefreshLocations();
}


float ATableActor::GetTableHeight() const
{
	return LEG_LENGTH + TABLE_TOP_THICKNESS;
}


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
	TableSize = FVector2D(
		FVector::Distance(Corners[0]->GetComponentLocation(), Corners[1]->GetComponentLocation()),
		FVector::Distance(Corners[0]->GetComponentLocation(), Corners[3]->GetComponentLocation())
	);

	

	// HACK: fix root location
	FVector NewRelativeRoot = FVector(
		Corners[0]->GetRelativeTransform().GetLocation().X - TableSize.X / 2,
		Corners[0]->GetRelativeTransform().GetLocation().Y - TableSize.Y / 2,
		0
	);
	FVector NewWorldRoot = this->GetTransform().TransformPosition(NewRelativeRoot);

	// Refresh countertop geometry
	CounterTop->Build(FVector(TableSize, TABLE_TOP_THICKNESS), false);
	CounterTop->SetWorldLocation(NewWorldRoot+FVector(0,0, LEG_LENGTH  + TABLE_TOP_THICKNESS/2));

	// Refresh legs location
	const TArray<FVector> LegsOffsets = {
	{-LEG_SIDE_SIZE / 2, -LEG_SIDE_SIZE / 2, -LEG_LENGTH / 2 - ANCHOR_HOVER_DISTANCE - TABLE_TOP_THICKNESS},
	{ LEG_SIDE_SIZE / 2, -LEG_SIDE_SIZE / 2, -LEG_LENGTH / 2 - ANCHOR_HOVER_DISTANCE - TABLE_TOP_THICKNESS},
	{LEG_SIDE_SIZE / 2, LEG_SIDE_SIZE / 2, -LEG_LENGTH / 2 - ANCHOR_HOVER_DISTANCE - TABLE_TOP_THICKNESS},
	{-LEG_SIDE_SIZE / 2, LEG_SIDE_SIZE / 2, -LEG_LENGTH / 2 - ANCHOR_HOVER_DISTANCE - TABLE_TOP_THICKNESS} };
	for (size_t i = 0; i < 4; i++)
		Legs[i]->SetRelativeLocation(Corners[i]->GetRelativeTransform().GetLocation() + LegsOffsets[i]);

	// Remove old chairs
	TArray<AActor*> SpawnedChairs;
	this->GetAllChildActors(SpawnedChairs);
	for (size_t i = 0; i < SpawnedChairs.Num(); i++)
		SpawnedChairs[i]->Destroy();

	// Spawn necessary chairs
	constexpr float CHAIRS_DISTANCE_FROM_TABLE = 25;
	constexpr float DISTANCE_BETWEEN_CHAIRS = 50;
	constexpr float CHAIRS_INTERVAL = (AProceduralChair::CHAIR_SQUARE_SIZE + DISTANCE_BETWEEN_CHAIRS);

	

	int ChairsToSpawnOnYSide = TableSize.Y / CHAIRS_INTERVAL;
	int ChairsToSpawnOnXSide = TableSize.X / CHAIRS_INTERVAL;

	// The offset to add so the chairs line is centered
	// Remaining space / 2
	const float CHAIRS_YLINE_LENGTH = ChairsToSpawnOnYSide * AProceduralChair::CHAIR_SQUARE_SIZE + (ChairsToSpawnOnYSide-1)*DISTANCE_BETWEEN_CHAIRS;
	const float YChairsSpawnOffset = (TableSize.Y - CHAIRS_YLINE_LENGTH ) / 2;
	

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
	const float XChairsSpawnOffset = (TableSize.X - CHAIRS_XLINE_LENGTH) / 2;

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

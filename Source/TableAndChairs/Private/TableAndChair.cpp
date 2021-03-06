


#include "TableAndChair.h"

const float		ATableAndChair::DISTANCE_BETWEEN_CHAIRS = 50;
const float		ATableAndChair::CHAIRS_DISTANCE_FROM_TABLE = 25;
const float		ATableAndChair::CHAIRS_INTERVAL = (AProceduralChair::CHAIR_SQUARE_SIZE + DISTANCE_BETWEEN_CHAIRS);
const float		ATableAndChair::TABLE_MIN_SIZE = (AProceduralChair::CHAIR_SQUARE_SIZE + DISTANCE_BETWEEN_CHAIRS * 2);
const int		ATableAndChair::MAX_CHAIRS = 100;
const float		ATableAndChair::TABLE_MAX_SIZE = TABLE_MIN_SIZE * MAX_CHAIRS - DISTANCE_BETWEEN_CHAIRS * (MAX_CHAIRS - 1);


// Sets default values
ATableAndChair::ATableAndChair()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	this->SetRootComponent(RootComponent);
}


void ATableAndChair::SpawnChairs(int HowMany, TArray<FChairCuple>& Container)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int y = 0; y < HowMany; ++y)
	{
		AProceduralChair* Chair = GetWorld()->SpawnActor<AProceduralChair>(AProceduralChair::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator, SpawnParameters);

		AProceduralChair* MirroredChair = GetWorld()->SpawnActor<AProceduralChair>(AProceduralChair::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator, SpawnParameters);

		Chair->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		MirroredChair->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

		FChairCuple BothChairs;
		BothChairs.BaseChair = Chair;
		BothChairs.MirrorChair = MirroredChair;
		Container.Add(BothChairs);
	}
}

int ATableAndChair::NumChairsToSpawn(int HowMany, TArray<FChairCuple>& Container)
{
	int LenContainer = Container.Num();

	if (HowMany > LenContainer)
	{
		HowMany -= LenContainer;
	}
	else
	{
		HowMany = LenContainer - HowMany;
		for (int i = HowMany; i-- > 0; )
		{
			Container[i].Delete();
			Container.RemoveAt(i);
		}

		HowMany = 0;
	}

	return HowMany;
}


// Called when the game starts or when spawned
void ATableAndChair::BeginPlay()
{
	Super::BeginPlay();

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	Table = GetWorld()->SpawnActor<AProceduralTable>(AProceduralTable::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	Table->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	
	Corners = GetWorld()->SpawnActor<ACornerActor>(ACornerActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParameters);
	Corners->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

	Corners->GetCorner(0)->SetRelativeLocation(FVector( Table->GetTableSize().X * 0.5f,  
		Table->GetTableSize().Y * 0.5f, 
		Table->GetTableHeight() + ACornerActor::ANCHOR_HOVER_DISTANCE));

	Corners->GetCorner(1)->SetRelativeLocation(FVector(-Table->GetTableSize().X * 0.5f,  
		Table->GetTableSize().Y * 0.5f, 
		Table->GetTableHeight() + ACornerActor::ANCHOR_HOVER_DISTANCE));

	Corners->GetCorner(2)->SetRelativeLocation(FVector(-Table->GetTableSize().X * 0.5f, 
		-Table->GetTableSize().Y * 0.5f, 
		Table->GetTableHeight() + ACornerActor::ANCHOR_HOVER_DISTANCE));

	Corners->GetCorner(3)->SetRelativeLocation(FVector( Table->GetTableSize().X * 0.5f, 
		-Table->GetTableSize().Y * 0.5f,
		Table->GetTableHeight() + ACornerActor::ANCHOR_HOVER_DISTANCE));

	//DrawDebugBox(GetWorld(), OverlapBox->GetComponentLocation(), FVector(Table->GetTableSize(), Table->GetTableHeight()), FColor::Purple, true, -1, 0, 10);

	RefreshLocations();
	
}

// Called every frame
void ATableAndChair::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ACornerActor* ATableAndChair::GetCornerActor()
{
	return Corners;
}

void ATableAndChair::BeginDestroy()
{
	Super::BeginDestroy();

	if (!Table->IsValidLowLevel()) return;
	if (Table)
	{
		Table->ConditionalBeginDestroy();
		Table->Destroy();
	}

	if (!Corners->IsValidLowLevel()) return;
	if (Corners)
	{
		Corners->ConditionalBeginDestroy();
		Corners->Destroy();
	}

	for (size_t i = 0; i < LeftRightChairs.Num(); ++i)
	{
		LeftRightChairs[i].Delete();
	}
	LeftRightChairs.Empty();

	for (size_t i = 0; i < FrontBackChairs.Num(); ++i)
	{
		FrontBackChairs[i].Delete();
	}
	FrontBackChairs.Empty();
}

bool ATableAndChair::SetCornerWorldLocation(UProceduralMeshComponent* Corner, const FVector NewCornerWorldLocation)
{
	if (Corner == nullptr)
		return false;

	const FVector OldLocation = Corner->GetComponentLocation();

	//// The current corner location is where the mouse ray hits the table plane
	Corner->SetWorldLocation(NewCornerWorldLocation);

	FVector CurrentSign = (NewCornerWorldLocation - Corners->GetOppositeCornerLocation()).GetSignVector();

	UProceduralMeshComponent* ClockwiseCorner = Corners->GetFixedXCorner(Corner);
	UProceduralMeshComponent* CounterClockwiseCorner = Corners->GetFixedYCorner(Corner);

	const FVector Corner1OldLoc = ClockwiseCorner->GetComponentLocation();
	const FVector Corner2OldLoc = CounterClockwiseCorner->GetComponentLocation();

	ClockwiseCorner->SetWorldLocation(FVector(Corner1OldLoc.X, NewCornerWorldLocation.Y, Corner1OldLoc.Z));
	CounterClockwiseCorner->SetWorldLocation(FVector(NewCornerWorldLocation.X, Corner2OldLoc.Y, Corner2OldLoc.Z));

	const float DistanceClockwise = FVector::Distance(NewCornerWorldLocation, ClockwiseCorner->GetComponentLocation());
	const float DistanceCounterClockwise = FVector::Distance(NewCornerWorldLocation, CounterClockwiseCorner->GetComponentLocation());

	//// Minimum table area check
	if (DistanceClockwise < TABLE_MIN_SIZE ||
		DistanceCounterClockwise < TABLE_MIN_SIZE ||
		DistanceClockwise > TABLE_MAX_SIZE ||
		DistanceCounterClockwise > TABLE_MAX_SIZE ||
		CurrentSign != Corners->GetSelectedCornerSign()
		)
	{
		Corner->SetWorldLocation(OldLocation);
		ClockwiseCorner->SetWorldLocation(Corner1OldLoc);
		CounterClockwiseCorner->SetWorldLocation(Corner2OldLoc);
		return false;
	}
	RefreshLocations();

	return true;
}

void ATableAndChair::RefreshLocations()
{
	FVector ChairsForwardSpawnPoint = Corners->GetCorner(2)->GetComponentLocation();
	FVector ChairsBackwardSpawnPoint = Corners->GetCorner(3)->GetComponentLocation();

	FVector ChairsLeftSpawnPoint = ChairsForwardSpawnPoint;
	FVector ChairsRightSpawnPoint = Corners->GetCorner(1)->GetComponentLocation();
	float HalfChairSquareSize = AProceduralChair::CHAIR_SQUARE_SIZE * 0.5f;

	// Cache table size
	FVector2D TmpTableSize = FVector2D(
		FVector::Distance(Corners->GetCorner(0)->GetComponentLocation(), ChairsRightSpawnPoint),
		FVector::Distance(Corners->GetCorner(0)->GetComponentLocation(), ChairsBackwardSpawnPoint)
	);

	FVector NewRelativeRoot = FVector(
		Corners->GetCorner(0)->GetRelativeLocation().X - Table->GetTableSize().X * 0.5f,
		Corners->GetCorner(0)->GetRelativeLocation().Y - Table->GetTableSize().Y * 0.5f,
		Table->GetTableHeight()
	);
	FVector NewWorldRoot = this->GetTransform().TransformPosition(NewRelativeRoot);
	Table->UpdateTableWorldLocation(NewWorldRoot);
	Table->SetTableSize(TmpTableSize);


	// Get the number of chairs to spawn
	int ChairsToSpawnOnYSide = NumChairsToSpawn(Table->GetTableSize().Y / CHAIRS_INTERVAL, FrontBackChairs);
	int ChairsToSpawnOnXSide = NumChairsToSpawn(Table->GetTableSize().X / CHAIRS_INTERVAL, LeftRightChairs);

	// Spawn chairs forward and backward
	SpawnChairs(ChairsToSpawnOnYSide, FrontBackChairs);


	const float CHAIRS_YLINE_LENGTH = CHAIRS_INTERVAL * FrontBackChairs.Num() - DISTANCE_BETWEEN_CHAIRS;
	const float YChairsSpawnOffset = (Table->GetTableSize().Y - CHAIRS_YLINE_LENGTH) * 0.5f;


	ChairsForwardSpawnPoint.X -= CHAIRS_DISTANCE_FROM_TABLE;
	ChairsForwardSpawnPoint.Y += HalfChairSquareSize;

	
	ChairsBackwardSpawnPoint.X += CHAIRS_DISTANCE_FROM_TABLE;
	ChairsBackwardSpawnPoint.Y += HalfChairSquareSize;

	// Update chairs location
	for (int y = 0; y < FrontBackChairs.Num(); ++y)
	{
		FrontBackChairs[y].BaseChair->SetActorLocation(FVector(ChairsForwardSpawnPoint.X, YChairsSpawnOffset + ChairsForwardSpawnPoint.Y + y * CHAIRS_INTERVAL, this->GetActorLocation().Z));
		FrontBackChairs[y].MirrorChair->SetActorLocationAndRotation(FVector(ChairsBackwardSpawnPoint.X, YChairsSpawnOffset + ChairsBackwardSpawnPoint.Y + y * CHAIRS_INTERVAL, this->GetActorLocation().Z), FRotator(0, -180, 0));
	}


	//  Spawn chairs left right
	SpawnChairs(ChairsToSpawnOnXSide, LeftRightChairs);

	const float CHAIRS_XLINE_LENGTH = CHAIRS_INTERVAL * LeftRightChairs.Num() - DISTANCE_BETWEEN_CHAIRS;
	const float XChairsSpawnOffset = (Table->GetTableSize().X - CHAIRS_XLINE_LENGTH) * 0.5f;

	ChairsLeftSpawnPoint.Y -= CHAIRS_DISTANCE_FROM_TABLE;
	ChairsLeftSpawnPoint.X += HalfChairSquareSize;
	
	ChairsRightSpawnPoint.Y += CHAIRS_DISTANCE_FROM_TABLE;
	ChairsRightSpawnPoint.X += HalfChairSquareSize;

	// Update chairs location
	for (int x = 0; x < LeftRightChairs.Num(); ++x)
	{
		LeftRightChairs[x].BaseChair->SetActorLocationAndRotation(FVector(XChairsSpawnOffset + ChairsLeftSpawnPoint.X + x * CHAIRS_INTERVAL, ChairsLeftSpawnPoint.Y, this->GetActorLocation().Z), FRotator(0, 90, 0));
		LeftRightChairs[x].MirrorChair->SetActorLocationAndRotation(FVector(XChairsSpawnOffset + ChairsRightSpawnPoint.X + x * CHAIRS_INTERVAL, ChairsRightSpawnPoint.Y, this->GetActorLocation().Z), FRotator(0, -90, 0));
	}
}

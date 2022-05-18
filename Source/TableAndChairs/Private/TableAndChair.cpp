


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
	for (int y = 0; y < HowMany; ++y)
	{
		AProceduralChair* Chair = GetWorld()->SpawnActor<AProceduralChair>(AProceduralChair::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator);

		AProceduralChair* MirroredChair = GetWorld()->SpawnActor<AProceduralChair>(AProceduralChair::StaticClass(),
			FVector::ZeroVector,
			FRotator::ZeroRotator);

		Chair->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
		MirroredChair->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

		FChairCuple BothChairs;
		BothChairs.BaseChair = Chair;
		BothChairs.MirrorChair = MirroredChair;
		Container.Add(BothChairs);
	}
}


// Called when the game starts or when spawned
void ATableAndChair::BeginPlay()
{
	Super::BeginPlay();

	Table = GetWorld()->SpawnActor<AProceduralTable>(AProceduralTable::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	Table->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);
	
	Corners = GetWorld()->SpawnActor<ACornerActor>(ACornerActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	Corners->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);

	Corners->GetCorner(0)->SetRelativeLocation(FVector( Table->GetTableSize().X / 2,  Table->GetTableSize().Y / 2, Table->GetTableHeight() + ACornerActor::ANCHOR_HOVER_DISTANCE));
	Corners->GetCorner(1)->SetRelativeLocation(FVector(-Table->GetTableSize().X / 2,  Table->GetTableSize().Y / 2, Table->GetTableHeight() + ACornerActor::ANCHOR_HOVER_DISTANCE));
	Corners->GetCorner(2)->SetRelativeLocation(FVector(-Table->GetTableSize().X / 2, -Table->GetTableSize().Y / 2, Table->GetTableHeight() + ACornerActor::ANCHOR_HOVER_DISTANCE));
	Corners->GetCorner(3)->SetRelativeLocation(FVector( Table->GetTableSize().X / 2, -Table->GetTableSize().Y / 2, Table->GetTableHeight() + ACornerActor::ANCHOR_HOVER_DISTANCE));

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



void ATableAndChair::RefreshLocations()
{
	// Cache table size
	FVector2D TmpTableSize = FVector2D(
		FVector::Distance(Corners->GetCorner(0)->GetComponentLocation(), Corners->GetCorner(1)->GetComponentLocation()),
		FVector::Distance(Corners->GetCorner(0)->GetComponentLocation(), Corners->GetCorner(3)->GetComponentLocation())
	);

	FVector NewRelativeRoot = FVector(
		Corners->GetCorner(0)->GetRelativeLocation().X - Table->GetTableSize().X / 2,
		Corners->GetCorner(0)->GetRelativeLocation().Y - Table->GetTableSize().Y / 2,
		Table->GetTableHeight()
	);
	FVector NewWorldRoot = this->GetTransform().TransformPosition(NewRelativeRoot);
	Table->UpdateTableWorldLocation(NewWorldRoot);
	Table->SetTableSize(TmpTableSize);



	int ChairsToSpawnOnYSide = Table->GetTableSize().Y / CHAIRS_INTERVAL;
	int ChairsToSpawnOnXSide = Table->GetTableSize().X / CHAIRS_INTERVAL;

	if (ChairsToSpawnOnYSide > FrontBackChairs.Num())
	{
		ChairsToSpawnOnYSide -= FrontBackChairs.Num();
	}
	else
	{
		ChairsToSpawnOnYSide = FrontBackChairs.Num() - ChairsToSpawnOnYSide;
		for (int i = ChairsToSpawnOnYSide; i-- > 0; )
		{
			FrontBackChairs[i].Delete();
			FrontBackChairs.RemoveAt(i);
		}

		ChairsToSpawnOnYSide = 0;
	}


	if (ChairsToSpawnOnXSide > LeftRightChairs.Num())
	{
		ChairsToSpawnOnXSide -= LeftRightChairs.Num();
	}
	else
	{
		ChairsToSpawnOnXSide = LeftRightChairs.Num() - ChairsToSpawnOnXSide;
		for (int i = ChairsToSpawnOnXSide; i-- > 0; )
		{
			LeftRightChairs[i].Delete();
			LeftRightChairs.RemoveAt(i);
		}

		ChairsToSpawnOnXSide = 0;
	}



	// Spawn chairs forward and backward
	SpawnChairs(ChairsToSpawnOnYSide, FrontBackChairs);


	// The offset to add so the chairs line is centered
	// Remaining space / 2
	const float CHAIRS_YLINE_LENGTH = FrontBackChairs.Num() * AProceduralChair::CHAIR_SQUARE_SIZE + (FrontBackChairs.Num() - 1) * DISTANCE_BETWEEN_CHAIRS;
	const float YChairsSpawnOffset = (Table->GetTableSize().Y - CHAIRS_YLINE_LENGTH) / 2;

	FVector ChairsForwardSpawnPoint = Corners->GetCorner(2)->GetComponentLocation();
	ChairsForwardSpawnPoint.X -= CHAIRS_DISTANCE_FROM_TABLE;
	ChairsForwardSpawnPoint.Y += AProceduralChair::CHAIR_SQUARE_SIZE / 2;

	FVector ChairsBackwardSpawnPoint = Corners->GetCorner(3)->GetComponentLocation();
	ChairsBackwardSpawnPoint.X += CHAIRS_DISTANCE_FROM_TABLE;
	ChairsBackwardSpawnPoint.Y += AProceduralChair::CHAIR_SQUARE_SIZE / 2;

	// Update chairs location
	for (int y = 0; y < FrontBackChairs.Num(); ++y)
	{
		FrontBackChairs[y].BaseChair->SetActorLocation(FVector(ChairsForwardSpawnPoint.X, YChairsSpawnOffset + ChairsForwardSpawnPoint.Y + y * CHAIRS_INTERVAL, this->GetActorLocation().Z));
		FrontBackChairs[y].MirrorChair->SetActorLocationAndRotation(FVector(ChairsBackwardSpawnPoint.X, YChairsSpawnOffset + ChairsBackwardSpawnPoint.Y + y * CHAIRS_INTERVAL, this->GetActorLocation().Z), FRotator(0, -180, 0));
	}


	//  Spawn chairs left right
	SpawnChairs(ChairsToSpawnOnXSide, LeftRightChairs);

	const float CHAIRS_XLINE_LENGTH = LeftRightChairs.Num() * AProceduralChair::CHAIR_SQUARE_SIZE + (LeftRightChairs.Num() - 1) * DISTANCE_BETWEEN_CHAIRS;
	const float XChairsSpawnOffset = (Table->GetTableSize().X - CHAIRS_XLINE_LENGTH) / 2;

	// Left side
	FVector ChairsLeftSpawnPoint = Corners->GetCorner(2)->GetComponentLocation();
	ChairsLeftSpawnPoint.Y -= CHAIRS_DISTANCE_FROM_TABLE;
	ChairsLeftSpawnPoint.X += AProceduralChair::CHAIR_SQUARE_SIZE / 2;

	FVector ChairsRightSpawnPoint = Corners->GetCorner(1)->GetComponentLocation();
	ChairsRightSpawnPoint.Y += CHAIRS_DISTANCE_FROM_TABLE;
	ChairsRightSpawnPoint.X += AProceduralChair::CHAIR_SQUARE_SIZE / 2;


	// Update chairs location
	for (int x = 0; x < LeftRightChairs.Num(); ++x)
	{
		LeftRightChairs[x].BaseChair->SetActorLocationAndRotation(FVector(XChairsSpawnOffset + ChairsLeftSpawnPoint.X + x * CHAIRS_INTERVAL, ChairsLeftSpawnPoint.Y, this->GetActorLocation().Z), FRotator(0, 90, 0));
		LeftRightChairs[x].MirrorChair->SetActorLocationAndRotation(FVector(XChairsSpawnOffset + ChairsRightSpawnPoint.X + x * CHAIRS_INTERVAL, ChairsRightSpawnPoint.Y, this->GetActorLocation().Z), FRotator(0, -90, 0));
	}
}

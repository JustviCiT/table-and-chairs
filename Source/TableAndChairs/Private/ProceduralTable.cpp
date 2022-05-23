


#include "ProceduralTable.h"

const float		AProceduralTable::LEG_LENGTH = 65;
const float		AProceduralTable::LEG_SIDE_SIZE = 10;
const float		AProceduralTable::TABLE_TOP_THICKNESS = 10.0f;
const FVector2D AProceduralTable::DEFAULT_TABLE_SIZE = FVector2D(200, 200);
const float		AProceduralTable::DEFAULT_TABLE_HEIGHT = AProceduralTable::LEG_LENGTH + AProceduralTable::TABLE_TOP_THICKNESS;

AProceduralTable::AProceduralTable()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// Load table material
	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Materials/GenericMaterial.GenericMaterial'"));
	if (Material.Succeeded())
	{
		TableMaterial = Material.Object;
	}
	else
	{
		UE_LOG(LogTaC, Error, TEXT("Table material failed loading"));
	}

	// Create countertop
	CounterTop = CreateDefaultSubobject<UProceduralBoxComponent>(TEXT("Countertop"));
	CounterTop->SetupAttachment(RootComponent);
	CounterTop->Build(FVector(TableSize, TABLE_TOP_THICKNESS),true);
	CounterTop->SetBoxMaterial(TableMaterial);
	CounterTop->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CounterTop->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Create legs
	LegsOffsets = {
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 },
		{ 0,0,0 }
	};

	for (size_t i = 0; i < LegsOffsets.Num(); i++)
	{
		const FString LegName = "Leg" + FString::FromInt(i);
		auto LegComp = CreateDefaultSubobject<UProceduralBoxComponent>(*LegName);
		LegComp->Build(FVector(LEG_SIDE_SIZE, LEG_SIDE_SIZE, LEG_LENGTH));
		LegComp->SetupAttachment(CounterTop);
		LegComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
		LegComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Legs.Add(LegComp);
	}

	UpdateLegsLocation();
}

void AProceduralTable::UpdateLegsLocation()
{
	const float tmpTableX = TableSize.X * 0.5f;
	const float tmpTableY = TableSize.Y * 0.5f;
	const float tmpLegWidth = LEG_SIDE_SIZE * 0.5f;
	const float tmpLegHeight = LEG_LENGTH * 0.5f + TABLE_TOP_THICKNESS * 0.5f;

	LegsOffsets = {
		{ tmpTableX - tmpLegWidth, tmpTableY - tmpLegWidth, -tmpLegHeight },
		{ tmpTableX - tmpLegWidth,-tmpTableY + tmpLegWidth, -tmpLegHeight },
		{-tmpTableX + tmpLegWidth, tmpTableY - tmpLegWidth, -tmpLegHeight },
		{-tmpTableX + tmpLegWidth,-tmpTableY + tmpLegWidth, -tmpLegHeight }
	};

	// Refresh legs location
	for (size_t i = 0; i < LegsOffsets.Num(); i++)
		Legs[i]->SetRelativeLocation(LegsOffsets[i]);
}

void AProceduralTable::BeginPlay()
{
	Super::BeginPlay();
}

FVector2D AProceduralTable::GetTableSize() const
{
	return TableSize;
}

void AProceduralTable::SetTableSize(const FVector2D& pSize)
{
	TableSize = pSize;

	// Refresh countertop geometry
	CounterTop->Build(FVector(TableSize, TABLE_TOP_THICKNESS), true);
	UpdateLegsLocation();
}


float AProceduralTable::GetTableHeight() const
{
	return LEG_LENGTH + TABLE_TOP_THICKNESS;
}

void AProceduralTable::UpdateTableWorldLocation(FVector& NewWorldLocation)
{
	CounterTop->SetWorldLocation(NewWorldLocation );
}


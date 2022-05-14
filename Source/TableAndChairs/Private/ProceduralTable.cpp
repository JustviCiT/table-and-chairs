


#include "ProceduralTable.h"

const float		AProceduralTable::LEG_LENGTH = 65;
const float		AProceduralTable::LEG_SIDE_SIZE = 10;
const bool		AProceduralTable::SHOULD_HAVE_COLLISION = false;
const float		AProceduralTable::TABLE_TOP_THICKNESS = 10.0f;
const FVector2D AProceduralTable::DEFAULT_SIZE = FVector2D(200, 200);

AProceduralTable::AProceduralTable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

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
	CounterTop->Build(FVector(TableSize, TABLE_TOP_THICKNESS));
	CounterTop->SetRelativeLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetTableHeight()));
	CounterTop->SetBoxMaterial(TableMaterial);


	// Create legs


	for (size_t i = 0; i < LegsOffsets.Num(); i++)
	{
		const FString LegName = "Leg" + FString::FromInt(i);
		auto LegComp = CreateDefaultSubobject<UProceduralBoxComponent>(*LegName);
		LegComp->Build(FVector(LEG_SIDE_SIZE, LEG_SIDE_SIZE, LEG_LENGTH));
		LegComp->SetupAttachment(RootComponent);
		LegComp->SetRelativeLocation(LegsOffsets[i]);
		Legs.Add(LegComp);
	}

}

void AProceduralTable::BeginPlay()
{
	Super::BeginPlay();
	
}

void AProceduralTable::UpdateLegsLocation()
{
	const float tmpTableX = TableSize.X * 0.5f;
	const float tmpTableY = TableSize.Y * 0.5f;
	const float tmpLegWidth = LEG_SIDE_SIZE * 0.5f;
	const float tmpLegHeight = LEG_LENGTH * 0.5f + TABLE_TOP_THICKNESS * 0.5f;

	LegsOffsets = {
		{ tmpTableX - tmpLegWidth, tmpTableY - tmpLegWidth, tmpLegHeight },
		{ tmpTableX - tmpLegWidth,-tmpTableY + tmpLegWidth, tmpLegHeight },
		{-tmpTableX + tmpLegWidth, tmpTableY - tmpLegWidth, tmpLegHeight },
		{-tmpTableX + tmpLegWidth,-tmpTableY + tmpLegWidth, tmpLegHeight }
	};

	// Refresh legs location
	for (size_t i = 0; i < LegsOffsets.Num(); i++)
		Legs[i]->SetRelativeLocation(LegsOffsets[i]);
}

FVector2D AProceduralTable::GetTableSize() const
{
	return TableSize;
}

void AProceduralTable::SetTableSize(const FVector2D& pSize)
{
	TableSize = pSize;

	// Refresh countertop geometry
	CounterTop->Build(FVector(TableSize, TABLE_TOP_THICKNESS));

	CounterTop->SetWorldLocation(/*NewWorldRoot + */ GetActorLocation());

	UpdateLegsLocation();
}


float AProceduralTable::GetTableHeight() const
{
	return LEG_LENGTH + TABLE_TOP_THICKNESS;
}


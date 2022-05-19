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

void ACornerActor::BeginPlay()
{
	Super::BeginPlay();
}

FVector ACornerActor::GetOppositeCornerLocation()
{
	return OppositeCornerLocation;
}

FVector ACornerActor::GetSelectedCornerSign()
{
	return SelectedCornerSign;
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

	OppositeCornerLocation = GetOppositeCorner(CurrentCorner)->GetComponentLocation();
	SelectedCornerSign = (CurrentCorner->GetComponentLocation() - OppositeCornerLocation).GetSignVector();
	Corners[index]->SetBoxMaterial(CornerMaterialSelected);
}

void ACornerActor::SetCornerEnabled(const UProceduralMeshComponent* CurrentCorner)
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE) {
		UE_LOG(LogTaC, Error, TEXT("CurrentCorner index is oob"));
		return;
	}

	OppositeCornerLocation = FVector(0.0f, 0.0f, 0.0f);
	SelectedCornerSign = FVector(0.0f, 0.0f, 0.0f);
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

UProceduralMeshComponent* ACornerActor::GetFixedXCorner(const UProceduralMeshComponent* CurrentCorner) const
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE)
		return nullptr;

	if ((index % 2) == 0)
		return Corners[(index + 1) % 4];
	else
		return Corners[(index - 1) % 4];
}

UProceduralMeshComponent* ACornerActor::GetFixedYCorner(const UProceduralMeshComponent* CurrentCorner) const
{
	int index = Corners.IndexOfByKey(CurrentCorner);
	if (index == INDEX_NONE || index > 3)
		return nullptr;

	return Corners[3 - index];
}

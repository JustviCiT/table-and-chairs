#include "ProceduralBoxComponent.h"


UProceduralBoxComponent::UProceduralBoxComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseAsyncCooking = true; // Use PhysX multithreaded cooking
}


void UProceduralBoxComponent::Build(const FVector Size, bool CollisionEnabled)
{
	Build(Size, TArray<FLinearColor>(), TArray<FProcMeshTangent>(), CollisionEnabled);
}

void UProceduralBoxComponent::Build(const FVector Size, const TArray<FLinearColor> LinearColors, const TArray<FProcMeshTangent> MeshTangents, bool CollisionEnabled )
{
	ensureAlwaysMsgf(!Size.IsZero(), TEXT("Size can't be zero"));
	ensureAlwaysMsgf(Size.X > 0 && Size.Y > 0, TEXT("Size can't be negative"));

	TArray<FVector2D> UV0 = { { 0, 0 } , { 1, 0 }, { 0, 1 }, { 1, 1 } };

	const float tmpX = Size.X * 0.5f;
	const float tmpY = Size.Y * 0.5f;
	const float tmpZ = Size.Z * 0.5f;

	// Top
	// Vertices are clockwise
	TArray<FVector> vertices = {
		{-tmpX, -tmpY, tmpZ },
		{ tmpX, -tmpY, tmpZ },
		{ tmpX,  tmpY, tmpZ },
		{ -tmpX,  tmpY, tmpZ }
	};

	// Triangles are counterclockwise
	TArray<int32> triangles = { 0, 3, 2, 2, 1, 0 };
	TArray<FVector> normals = { FVector::UpVector , FVector::UpVector, FVector::UpVector , FVector::UpVector };

	CreateMeshSection_LinearColor(0, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Bottom
	vertices = {
		{-tmpX, -tmpY, -tmpZ },
		{-tmpX,  tmpY, -tmpZ },
		{ tmpX, -tmpY, -tmpZ },
		{ tmpX,  tmpY, -tmpZ }
	};


	triangles = { 2, 1, 0, 1, 2, 3 };
	normals = { -FVector::UpVector , -FVector::UpVector, -FVector::UpVector , -FVector::UpVector };
	CreateMeshSection_LinearColor(1, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Backward
	vertices = {
		{-tmpX, -tmpY,  tmpZ },
		{-tmpX,  tmpY,  tmpZ },
		{-tmpX, -tmpY, -tmpZ },
		{-tmpX,  tmpY, -tmpZ }
	};
	triangles = { 0, 2, 1, 1, 2, 3 };
	normals = { -FVector::ForwardVector , -FVector::ForwardVector, -FVector::ForwardVector , -FVector::ForwardVector };
	CreateMeshSection_LinearColor(2, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Forward
	vertices = {
		{tmpX, -tmpY,  tmpZ },
		{tmpX,  tmpY,  tmpZ },
		{tmpX, -tmpY, -tmpZ },
		{tmpX,  tmpY, -tmpZ }
	};
	triangles = { 2, 0, 1, 1, 3, 2 };
	normals = { FVector::ForwardVector , FVector::ForwardVector, FVector::ForwardVector , FVector::ForwardVector };
	CreateMeshSection_LinearColor(3, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Left
	vertices = {
		{ tmpX, -tmpY,  tmpZ },
		{-tmpX, -tmpY,  tmpZ },
		{ tmpX, -tmpY, -tmpZ },
		{-tmpX, -tmpY, -tmpZ }
	};
	triangles = { 0, 2, 1, 1, 2, 3 };
	normals = { -FVector::RightVector, -FVector::RightVector, -FVector::RightVector , -FVector::RightVector };
	CreateMeshSection_LinearColor(4, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);

	// Right
	vertices = {
		{ tmpX, tmpY,  tmpZ },
		{-tmpX, tmpY,  tmpZ },
		{ tmpX, tmpY, -tmpZ },
		{-tmpX, tmpY, -tmpZ }
	};
	triangles = { 0, 1, 2, 1, 3, 2 };
	normals = { FVector::RightVector, FVector::RightVector, FVector::RightVector , FVector::RightVector };
	CreateMeshSection_LinearColor(5, vertices, triangles, normals, UV0, LinearColors, MeshTangents, CollisionEnabled);
}

void UProceduralBoxComponent::SetBoxMaterial(const UMaterialInterface * const NewMaterial)
{
	for (size_t i = 0; i < GetNumSections(); i++)
		SetMaterial(i, (UMaterialInterface*)NewMaterial);
}

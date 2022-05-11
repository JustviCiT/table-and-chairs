#include "TableAndChairsPlayerController.h"

void ATableAndChairsPlayerController::ExitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

void ATableAndChairsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	bShowMouseCursor = true;
	SetActorTickInterval(0.05);

	// Bind the functions for clicking with the cursor to edit the table
	ensureMsgf(InputComponent != nullptr, TEXT("ATableAndChairsPlayerController can't bind mouse events because InputComponent is nullptr"));
	InputComponent->BindAction("MouseLeftClicked", IE_Pressed, this, &ATableAndChairsPlayerController::LeftClickPressed);
	InputComponent->BindAction("MouseLeftClicked", IE_Released, this, &ATableAndChairsPlayerController::LeftClickReleased);
	InputComponent->BindAction("Escape", IE_Pressed, this, &ATableAndChairsPlayerController::ExitGame);
}


void ATableAndChairsPlayerController::LeftClickPressed()
{
	UE_LOG(LogTaC, Log, TEXT("Left Click Pressed"));

	FVector Start;
	FVector ForwardVector;
	bool flag = DeprojectMousePositionToWorld(Start, ForwardVector);
	UE_LOG(LogTaC, Log, TEXT("START %s"), *Start.ToString());
	UE_LOG(LogTaC, Log, TEXT("FORWARD VECTOR %s"), *ForwardVector.ToString());
	if (!flag) return;

	FVector End = ((ForwardVector * EDITING_RAY_LENGTH) + Start);

	// Spawn a ray from the cursor to "infinity" to find an editable table
	FCollisionQueryParams CollisionParams;
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams) &&
		OutHit.bBlockingHit &&
		OutHit.GetActor()->GetClass() == ATableActor::StaticClass())
	{
		UE_LOG(LogTaC, Log, TEXT("this condition is true"));
		TableBeingEdited = (ATableActor*)OutHit.GetActor();
		CurrentCornerDraggedComponent = StaticCast<UProceduralMeshComponent*>(OutHit.GetComponent());

		TableBeingEdited->SetCornerSelected(CurrentCornerDraggedComponent);
	}
}

void ATableAndChairsPlayerController::LeftClickReleased()
{
	UE_LOG(LogTaC, Log, TEXT("Left Click released"));

	if (TableBeingEdited == nullptr || CurrentCornerDraggedComponent == nullptr) {
		//TODO REMOVE table
	} else {
		TableBeingEdited->SetCornerEnabled(CurrentCornerDraggedComponent);
	}

	TableBeingEdited = nullptr;
	CurrentCornerDraggedComponent = nullptr;
}

void ATableAndChairsPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TableBeingEdited == nullptr || CurrentCornerDraggedComponent == nullptr)
		return;

	// Intersect the mouse ray with the table plane to find the new corner
	FVector Start;
	FVector ForwardVector;
	bool flag = DeprojectMousePositionToWorld(Start, ForwardVector);
	if (!flag) return;

	FVector End = ((ForwardVector * EDITING_RAY_LENGTH) + Start);
	FVector NewCornerWorldLocation = FMath::LinePlaneIntersection(Start, End, CurrentCornerDraggedComponent->GetComponentLocation(), FVector::UpVector);

	TableBeingEdited->SetCornerWorldLocation(CurrentCornerDraggedComponent, NewCornerWorldLocation);
}

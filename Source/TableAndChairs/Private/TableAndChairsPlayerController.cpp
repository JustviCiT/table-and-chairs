#include "TableAndChairsPlayerController.h"


const float ATableAndChairsPlayerController::EDITING_RAY_LENGTH = 5000.0f;

void ATableAndChairsPlayerController::ExitGame()
{
	FGenericPlatformMisc::RequestExit(false);
}

void ATableAndChairsPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FirstCamera = Cast<ACamera>(GetPawn());
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
	InputComponent->BindAction("MouseRightClicked", IE_Released, this, &ATableAndChairsPlayerController::RightClickReleased);

	InputComponent->BindAxis("Zoom", this, &ATableAndChairsPlayerController::ZoomInOut);
	InputComponent->BindAxis("MoveUp", this, &ATableAndChairsPlayerController::MoveUp);
	InputComponent->BindAxis("MoveDown", this, &ATableAndChairsPlayerController::MoveDown);
	InputComponent->BindAxis("MoveLeft", this, &ATableAndChairsPlayerController::MoveLeft);
	InputComponent->BindAxis("MoveRight", this, &ATableAndChairsPlayerController::MoveRight);
}

void ATableAndChairsPlayerController::ZoomInOut(float AxisValue)
{
	MovementZoom = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	//UE_LOG(LogTaC, Log, TEXT("ZoomIn: %f"), MovementZoom);
}

void ATableAndChairsPlayerController::MoveUp(float AxisValue)
{
	MovementInputLeftUp.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	//UE_LOG(LogTaC, Log, TEXT("MoveUp: %s"), *MovementInputLeftUp.ToString());
}

void ATableAndChairsPlayerController::MoveDown(float AxisValue)
{
	MovementInputRightDown.Y = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	//UE_LOG(LogTaC, Log, TEXT("MoveDown: %s"), *MovementInputRightDown.ToString());
}

void ATableAndChairsPlayerController::MoveRight(float AxisValue)
{
	MovementInputRightDown.X = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	//UE_LOG(LogTaC, Log, TEXT("MoveRight: %s"), *MovementInputRightDown.ToString());
}

void ATableAndChairsPlayerController::MoveLeft(float AxisValue)
{
	MovementInputLeftUp.X = FMath::Clamp(AxisValue, -1.0f, 1.0f);
	//UE_LOG(LogTaC, Log, TEXT("MoveLeft: %s"), *MovementInputLeftUp.ToString());
}


void ATableAndChairsPlayerController::LeftClickPressed()
{
	UE_LOG(LogTaC, Log, TEXT("Left Click Pressed"));

	FVector Start;
	FVector ForwardVector;
	bool flag = DeprojectMousePositionToWorld(Start, ForwardVector);
	//UE_LOG(LogTaC, Log, TEXT("START %s"), *Start.ToString());
	//UE_LOG(LogTaC, Log, TEXT("FORWARD VECTOR %s"), *ForwardVector.ToString());
	if (!flag) return;

	FVector End = ((ForwardVector * EDITING_RAY_LENGTH) + Start);

	// Spawn a ray from the cursor to "infinity" to find an editable table
	FCollisionQueryParams CollisionParams;
	FHitResult OutHit;

	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams) &&
		OutHit.bBlockingHit)
	{
		UClass* ClassHit = OutHit.GetActor()->GetClass();
		//UE_LOG(LogTaC, Log, TEXT("CLASS NAME: %s"), *OutHit.GetActor()->GetClass()->GetName());

		if (ClassHit == ACornerActor::StaticClass())
		{
			TableBeingEdited = Cast<ATableAndChair>(OutHit.GetActor()->GetAttachParentActor());
			if (!TableBeingEdited)
			{
				UE_LOG(LogTaC, Error, TEXT("Table is null"));
				return;
			}

			CurrentCornerDraggedComponent = StaticCast<UProceduralMeshComponent*>(OutHit.GetComponent());
			//UE_LOG(LogTaC, Log, TEXT("component %s "), *OutHit.GetComponent()->GetName());
			TableBeingEdited->GetCornerActor()->SetCornerSelected(CurrentCornerDraggedComponent);
		}
		else
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
			GetWorld()->SpawnActor<ATableAndChair>(ATableAndChair::StaticClass(), OutHit.ImpactPoint, FRotator::ZeroRotator, SpawnParameters);
		}

	}
}

void ATableAndChairsPlayerController::LeftClickReleased()
{
	UE_LOG(LogTaC, Log, TEXT("Left Click released"));

	if (TableBeingEdited == nullptr || CurrentCornerDraggedComponent == nullptr) 
	{
		return;
	}
	
	TableBeingEdited->GetCornerActor()->SetCornerEnabled(CurrentCornerDraggedComponent);

	TableBeingEdited = nullptr;
	CurrentCornerDraggedComponent = nullptr;
}

void ATableAndChairsPlayerController::RightClickReleased()
{
	UE_LOG(LogTaC, Log, TEXT("Right Click Released"));

	FVector Start;
	FVector ForwardVector;
	bool flag = DeprojectMousePositionToWorld(Start, ForwardVector);
	if (!flag) return;

	FVector End = ((ForwardVector * EDITING_RAY_LENGTH) + Start);

	// Spawn a ray from the cursor to "infinity" to find an editable table
	FCollisionQueryParams CollisionParams;
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams) &&
		OutHit.bBlockingHit)
	{
		//UE_LOG(LogTaC, Log, TEXT("COMPONENT NAME: %s"), *->GetName());
		AActor* MainActor = OutHit.GetActor();
		AActor* ClassHit = MainActor->GetAttachParentActor() ;

		if (ClassHit != nullptr)
		{
			if (ClassHit->GetClass() == ATableAndChair::StaticClass()) 
			{
				if (!ClassHit->IsValidLowLevel()) return;
				ClassHit->ConditionalBeginDestroy();
				ClassHit->Destroy();
				UE_LOG(LogTaC, Log, TEXT(" Table actor kill from parent"));	
			}

		}
		else
		{
			
			if (MainActor->GetClass() == ATableAndChair::StaticClass())
			{
				if (!MainActor->IsValidLowLevel()) return;
				MainActor->ConditionalBeginDestroy();
				MainActor->Destroy();
				UE_LOG(LogTaC, Log, TEXT(" Table actor kill "));
			}

		}

	}
}

void ATableAndChairsPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FirstCamera && 
		( MovementZoom  != 0.0f ||
		 !MovementInputRightDown.IsZero() ||
		 !MovementInputLeftUp.IsZero()
		))
	{
		MovementZoom = MovementZoom * EDITING_RAY_LENGTH;
		MovementInputRightDown = MovementInputRightDown.GetSafeNormal() * EDITING_RAY_LENGTH;
		MovementInputLeftUp = MovementInputLeftUp.GetSafeNormal() * EDITING_RAY_LENGTH;
		
		FVector NewLocation = FirstCamera->GetActorLocation();
		NewLocation += FirstCamera->GetActorRightVector() * (MovementInputLeftUp.X + MovementInputRightDown.X) * DeltaTime;
		NewLocation += FirstCamera->GetActorUpVector() * (MovementInputLeftUp.Y + MovementInputRightDown.Y) * DeltaTime;
		NewLocation += FirstCamera->GetActorForwardVector() * MovementZoom * DeltaTime;
		FirstCamera->SetActorLocation(NewLocation);
	}

	if (TableBeingEdited == nullptr || CurrentCornerDraggedComponent == nullptr)
		return;

	// Intersect the mouse ray with the table plane to find the new corner
	FVector Start;
	FVector ForwardVector;
	bool flag = DeprojectMousePositionToWorld(Start, ForwardVector);
	if (!flag) return;

	FVector End = ((ForwardVector * EDITING_RAY_LENGTH) + Start);
	FVector NewCornerWorldLocation = FMath::LinePlaneIntersection(Start, End, CurrentCornerDraggedComponent->GetComponentLocation(), FVector::UpVector);

	bool Result = TableBeingEdited->GetCornerActor()->SetCornerWorldLocation(CurrentCornerDraggedComponent, NewCornerWorldLocation);

	if (Result)
	{
		TableBeingEdited->RefreshLocations();
	}
}

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProceduralTableWithChairs.h"
#include "LogTaC.h"
#include "Camera/CameraActor.h"
#include "Camera.h"
#include "TableAndChairsPlayerController.generated.h"

/**
 *	Player controller managing the editing of procedural tables
 */
UCLASS()
class TABLEANDCHAIRS_API ATableAndChairsPlayerController : public APlayerController
{
	GENERATED_BODY()


private:

	// This is the table currently being edited by the user
	UPROPERTY()
	ATableActor* TableBeingEdited;

	// This is the corner of the table currently being edited
	UPROPERTY()
	UProceduralMeshComponent* CurrentCornerDraggedComponent;

protected:
	virtual void BeginPlay() override; 

	virtual void SetupInputComponent() override;

	FVector2D MovementInputRightDown;
	FVector2D MovementInputLeftUp;
	float MovementZoom;

	UPROPERTY()
	ACamera* FirstCamera;

	UFUNCTION()
	void LeftClickPressed();

	UFUNCTION()
	void LeftClickReleased();

	UFUNCTION()
	void RightClickReleased();

	UFUNCTION()
	void ExitGame();

	UFUNCTION()
	void ZoomInOut(float AxisValue);

	UFUNCTION()
	void MoveUp(float AxisValue);

	UFUNCTION()
	void MoveDown(float AxisValue);

	UFUNCTION()
	void MoveRight(float AxisValue);

	UFUNCTION()
	void MoveLeft(float AxisValue);


public:
	// The length of the ray to find an editable table
	static const float EDITING_RAY_LENGTH;

	virtual void Tick(float DeltaTime) override;

};

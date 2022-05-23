#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Corner.h"
#include "TableAndChair.h"
#include "LogTaC.h"
#include "Camera/CameraActor.h"
#include "Camera.h"
#include "Kismet\KismetSystemLibrary.h"
#include "TableAndChairsPlayerController.generated.h"

/**
 *	Player controller managing the editing of procedural tables
 */
UCLASS()
class TABLEANDCHAIRS_API ATableAndChairsPlayerController : public APlayerController
{
	GENERATED_BODY()


private:

	/*
	This is the table currently being edited
	*/
	UPROPERTY()
	ATableAndChair* TableBeingEdited;

	/*
	This is the corner of the table currently being edited
	*/
	UPROPERTY()
	UProceduralMeshComponent* CurrentCornerDraggedComponent;

	/*
	@return True on Table overlapping on anything 
	*/
	UFUNCTION()
	bool TableIsOverlapping(FVector& SpawnPoint);

protected:
	virtual void BeginPlay() override; 

	virtual void SetupInputComponent() override;

	/*
	* Contains values used for camera movement
		X axis is Right, Y is Down 
	*/
	FVector2D MovementInputRightDown;

	/*
	* Contains values used for camera movement
		X axis is Left, Y is Up
	*/
	FVector2D MovementInputLeftUp;

	/*
	* Contains value used for camera movement
		+1 , -1 for ZoomIn/ZoomOut
	*/
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
	/*
	The length of the ray to find an editable table
	*/
	static const float EDITING_RAY_LENGTH;

	virtual void Tick(float DeltaTime) override;

};

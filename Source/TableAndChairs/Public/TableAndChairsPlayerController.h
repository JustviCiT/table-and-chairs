#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProceduralTableWithChairs.h"
#include "LogTaC.h"
#include "Camera/CameraActor.h"
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
	ATableActor* TableBeingEdited;

	// This is the corner of the table currently being edited
	UProceduralMeshComponent* CurrentCornerDraggedComponent;

	void LeftClickPressed();
	void LeftClickReleased();

	void RightClickReleased();

	void ExitGame();

protected:
	virtual void SetupInputComponent() override;

public:
	// The length of the ray to find an editable table
	static constexpr float EDITING_RAY_LENGTH = 10000.0f;

	virtual void Tick(float DeltaTime) override;

};

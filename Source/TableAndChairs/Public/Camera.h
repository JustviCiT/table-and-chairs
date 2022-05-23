

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera.generated.h"


/*
Camera pawn that is moved throught TableAndChairsPlayerController
*/
UCLASS()
class TABLEANDCHAIRS_API ACamera : public APawn
{
	GENERATED_BODY()

public:
	ACamera();

	UPROPERTY()
	USpringArmComponent* m_armComponent;

	UPROPERTY()
	UCameraComponent* m_cameraComponent;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

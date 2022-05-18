#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TableAndChairsPlayerController.h"
#include "Camera.h"
#include "TableAndChairsGameModeBase.generated.h"


UCLASS()
class TABLEANDCHAIRS_API ATableAndChairsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATableAndChairsGameModeBase();

protected:
	virtual void BeginPlay() override;

};

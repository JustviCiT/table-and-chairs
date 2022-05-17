

#pragma once

#include "CoreMinimal.h"
#include "ProceduralChairWithBackrest.h"
#include "ChairPair.generated.h"

USTRUCT()
struct FChairCuple
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	AProceduralChair* BaseChair;

	UPROPERTY(VisibleAnywhere)
	AProceduralChair* MirrorChair;

	void Delete()
	{
		if (BaseChair)
		{
			if (!BaseChair->IsValidLowLevel()) return;

			BaseChair->ConditionalBeginDestroy();
			BaseChair->Destroy();
		}

		if (MirrorChair)
		{
			if (!MirrorChair->IsValidLowLevel()) return;

			MirrorChair->ConditionalBeginDestroy();
			MirrorChair->Destroy();
		}
	}
};
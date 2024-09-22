#pragma once

#include "CoreMinimal.h"
#include "PlayerMovementState_ENUM.generated.h"

UENUM(BlueprintType)
enum EPlayerMovementState
{
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
	Crouching UMETA(DisplayName = "Crouching"),
	Sliding UMETA(DisplayName = "Sliding")
};
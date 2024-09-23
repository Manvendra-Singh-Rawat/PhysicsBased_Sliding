#pragma once

#include "CoreMinimal.h"
#include "PlayerMovementState_ENUM.generated.h"

UENUM(BlueprintType)
enum class EPlayerMovementState : uint8
{
	Walking UMETA(DisplayName = "Walking"),
	Running UMETA(DisplayName = "Running"),
	Crouching UMETA(DisplayName = "Crouching"),
	Sliding UMETA(DisplayName = "Sliding")
};
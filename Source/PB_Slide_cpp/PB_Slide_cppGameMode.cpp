// Copyright Epic Games, Inc. All Rights Reserved.

#include "PB_Slide_cppGameMode.h"
#include "PB_Slide_cppCharacter.h"
#include "UObject/ConstructorHelpers.h"

APB_Slide_cppGameMode::APB_Slide_cppGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

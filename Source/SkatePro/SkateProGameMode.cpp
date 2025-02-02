// Copyright Epic Games, Inc. All Rights Reserved.

#include "SkateProGameMode.h"
#include "SkateProCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASkateProGameMode::ASkateProGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

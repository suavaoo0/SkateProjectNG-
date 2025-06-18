// Copyright Epic Games, Inc. All Rights Reserved.

#include "NGprojectGameMode.h"
#include "NGprojectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANGprojectGameMode::ANGprojectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

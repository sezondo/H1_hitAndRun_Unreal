// Copyright Epic Games, Inc. All Rights Reserved.

#include "H1GameMode.h"
#include "H1Character.h"
#include "UObject/ConstructorHelpers.h"

AH1GameMode::AH1GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

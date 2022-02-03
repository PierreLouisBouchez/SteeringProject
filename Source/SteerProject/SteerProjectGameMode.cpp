// Copyright Epic Games, Inc. All Rights Reserved.

#include "SteerProjectGameMode.h"
#include "SteerProjectPlayerController.h"
#include "SteerProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASteerProjectGameMode::ASteerProjectGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ASteerProjectPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
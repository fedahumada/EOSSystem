// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/EOSSystemGameMode.h"
#include "Player/EOSSystemPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AEOSSystemGameMode::AEOSSystemGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AEOSSystemPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/Character/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/Player/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != nullptr)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}



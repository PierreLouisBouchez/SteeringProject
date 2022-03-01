// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SteerProjectPlayerController.generated.h"

UCLASS()
class ASteerProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ASteerProjectPlayerController();
	uint32 bMoveToMouseCursor : 1;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface


	/** Navigate player to the current mouse cursor location. */
	void MoveToMouseCursor();

	
	/** Input handlers for SetDestination action. */
	void OnSetDestinationPressed();
	void OnSetDestinationReleased();
};



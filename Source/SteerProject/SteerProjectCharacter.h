// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Vehicule.h"
#include "PlayableVehicule.h"
#include <SteerProject/Node.h>
#include <SteerProject/SceneBuilder.h>

#include "SteerProjectCharacter.generated.h"

UCLASS(Blueprintable)
class ASteerProjectCharacter :  public ACharacter
{
	
	GENERATED_BODY()

public:
	ASteerProjectCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	FVector targetVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Veloce)
	FVector CurrentVelocity;
	
	FVector seek(const FVector& target);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Path)
	ASceneBuilder* Scenebuilder;

	TArray<TArray<Node>> Map;
	
	TArray<FVector> targetList;
	int circuitindex = 0;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	FVector truncate(const FVector& vec, const float& m);



	virtual void BeginPlay() override;

	void PathFinding(FVector Target);
	int Manhattan(Node start, Node End);
	int FloorHundred(float a);

	void oneway();

};

